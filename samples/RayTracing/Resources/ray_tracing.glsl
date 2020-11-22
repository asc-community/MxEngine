in vec2 TexCoord;
out vec4 OutColor;

struct Box
{
    vec3 halfSize;
    mat3 rotation;
    vec3 position;
    vec3 albedo;
};

struct Sphere
{
    vec3 position;
    float radius;
    vec3 albedo;
};

struct LightSource
{
    vec3 position;
    float intensity;
};

uniform vec2 uViewportSize;
uniform vec3 uPosition;
uniform vec3 uDirection;
uniform vec3 uUp;
uniform float uFOV;
uniform float uTime;
uniform samplerCube uEnvironment;

#define HALF_PI (3.1415926535 / 2.0)
#define FAR_DISTANCE 1000000.0

vec3 GetRayDirection(vec2 texcoord, vec2 viewportSize, float fov, vec3 direction, vec3 up)
{
    vec2 texDiff = 0.5 * vec2(1.0 - 2.0 * texcoord.x, 2.0 * texcoord.y - 1.0);
    vec2 angleDiff = texDiff * vec2(viewportSize.x / viewportSize.y, 1.0) * tan(fov * 0.5);

    vec3 rayDirection = normalize(vec3(angleDiff, 1.0f));

    vec3 right = normalize(cross(up, direction));
    mat3 viewToWorld = mat3(
        right,
        up,
        direction
    );

    return viewToWorld * rayDirection;
}

bool IntersectRaySphere(vec3 origin, vec3 direction, Sphere sphere, out float fraction, out vec3 normal)
{
    vec3 L = origin - sphere.position;
    float a = dot(direction, direction);
    float b = 2.0 * dot(L, direction);
    float c = dot(L, L) - sphere.radius * sphere.radius;
    float D = b * b - 4 * a * c;
    if (D < 0.0)
        return false;
        
    fraction = (-b - sqrt(D)) / (2.0 * a);
    if (fraction < -sphere.radius)
        return false;

    normal = normalize(direction * fraction + L);

    return true;
}

bool IntersectRayBox(vec3 origin, vec3 direction, Box box, out float fraction, out vec3 normal)
{
    vec3 rd = box.rotation * direction;
    vec3 ro = box.rotation * origin - box.position;

    vec3 m = vec3(1.0) / rd; 

    vec3 s = vec3((rd.x < 0.0) ? 1.0 : -1.0,
                  (rd.y < 0.0) ? 1.0 : -1.0,
                  (rd.z < 0.0) ? 1.0 : -1.0);
    vec3 t1 = m * (-ro + s * box.halfSize);
    vec3 t2 = m * (-ro - s * box.halfSize);

    float tN = max(max(t1.x, t1.y), t1.z);
    float tF = min(min(t2.x, t2.y), t2.z);

    if (tN > tF || tF < 0.0) return false;

    mat3 txi = transpose(box.rotation);

    if (t1.x > t1.y && t1.x > t1.z)
        normal = txi[0] * s.x;
    else if (t1.y > t1.z)
        normal = txi[1] * s.y;
    else
        normal = txi[2] * s.z;

    fraction = tN;

    return true;
}

const int SphereCount = 3;
Sphere spheres[SphereCount];

const int BoxCount = 1;
Box boxes[BoxCount];

bool CastRay(vec3 rayOrigin, vec3 rayDirection, out float fraction, out vec3 normal, out vec3 color)
{
    float minDistance = FAR_DISTANCE;

    for (int i = 0; i < SphereCount; i++)
    {
        float fr;
        vec3 N;
        if (IntersectRaySphere(rayOrigin, rayDirection, spheres[i], fr, N) && fr < minDistance)
        {
            minDistance = fr;
            color = spheres[i].albedo;
            normal = N;
        }
    }

    for (int i = 0; i < BoxCount; i++)
    {
        float fr;
        vec3 N;
        if (IntersectRayBox(rayOrigin, rayDirection, boxes[i], fr, N) && fr < minDistance)
        {
            minDistance = fr;
            color = boxes[i].albedo;
            normal = N;
        }
    }

    fraction = minDistance;
    return minDistance != FAR_DISTANCE;
}

vec3 CalculateLightColor(vec3 fragmentPosition, vec3 fragmentNormal, vec3 fragmentColor, vec3 viewDirection, LightSource light)
{
    float lightDistance = length(light.position - fragmentPosition);
    vec3 lightDirection = normalize(light.position - fragmentPosition);
    float fraction;
    vec3 unusedN, unusedC;
    bool hasOcclusion = CastRay(fragmentPosition + fragmentNormal * 0.3, lightDirection, fraction, unusedN, unusedC);

    vec3 HDirection = normalize(lightDirection - viewDirection);

    float diffuseFactor = max(dot(fragmentNormal, lightDirection), 0.0);
    float specularFactor = pow(max(dot(fragmentNormal, HDirection), 0.0), 256.0);

    float shadow = 1.0 - float(hasOcclusion && fraction < lightDistance);
    shadow = shadow / clamp(lightDistance, 1.0, 10.0);

    return shadow * (diffuseFactor + specularFactor) * fragmentColor * light.intensity;
}

void main()
{
    spheres[0].position = vec3(5.0, 0.0, 0.0);
    spheres[1].position = vec3(5.0, 2.0, 0.0);
    spheres[2].position = vec3(3.0, 0.0, 2.0);
    spheres[0].radius = 0.5;
    spheres[1].radius = 0.5;
    spheres[2].radius = 0.5;
    spheres[0].albedo = vec3(0.6, 0.6, 0.6);
    spheres[1].albedo = vec3(1.0, 0.0, 0.0);
    spheres[2].albedo = vec3(0.9, 0.7, 0.0);

    boxes[0].albedo = vec3(0.9, 0.9, 0.9);
    boxes[0].halfSize = vec3(3.0, 0.1, 3.0);
    boxes[0].position = vec3(5.0, -1.0, 1.0);
    boxes[0].rotation = mat3(
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0
    );

    // animation
    spheres[0].position += 1.7 * vec3(sin(uTime), 0.0, cos(uTime));
    spheres[2].position += 1.7 * vec3(0.0, sin(2.0 * uTime) + 1.0, 0.0);

    const int LightsCount = 2;
    LightSource lights[LightsCount];
    lights[0].position = vec3( 0.0, 10.0,  0.0);
    lights[1].position = vec3( 5.0,  0.0,  0.0);
    lights[0].intensity = 0.8;
    lights[1].intensity = 0.8;
        
    vec3 rayDirection = GetRayDirection(TexCoord, uViewportSize, uFOV, uDirection, uUp);
    float hitFraction;
    vec3 hitPointNormal;
    vec3 hitPointColor;

    vec3 outputColor = vec3(0.0f);
    if (CastRay(uPosition, rayDirection, hitFraction, hitPointNormal, hitPointColor))
    {
        vec3 hitPointPosition = uPosition + rayDirection * hitFraction;
        
        vec3 reflectDirection = reflect(rayDirection, hitPointNormal);
        
        float fr;
        vec3 n;
        vec3 reflectionColor;
        bool hasReflectedHit = CastRay(hitPointPosition + hitPointNormal * 0.3, reflectDirection, fr, n, reflectionColor);

        vec3 totalReflection = vec3(0.0);

        for (int i = 0; i < LightsCount; i++)
        {
            outputColor += CalculateLightColor(hitPointPosition, hitPointNormal, hitPointColor, uDirection, lights[i]);
        }
        if (hasReflectedHit)
        {
            for (int i = 0; i < LightsCount; i++)
            {
                totalReflection += CalculateLightColor(hitPointPosition + fr * reflectDirection, n, reflectionColor, uDirection, lights[i]);
            }
        }
        else totalReflection = texture(uEnvironment, reflectDirection).rgb;
        outputColor *= totalReflection;
    }
    else
    {
        outputColor = texture(uEnvironment, rayDirection).rgb;
    }

    const float invGamma = 1.0 / 2.2;
    outputColor = pow(outputColor, vec3(invGamma));
    
    OutColor = vec4(outputColor, 1.0);
}