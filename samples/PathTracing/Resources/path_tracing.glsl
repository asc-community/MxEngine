in vec2 TexCoord;
out vec4 OutColor;

#define MATERIAL_DIFFUSE 0
#define MATERIAL_SPECULAR 1 
#define MATERIAL_REFRACTIVE 2
#define MATERIAL_TYPE int

struct Material
{
    vec3 emmitance;
    vec3 reflectance;
    MATERIAL_TYPE type;
};

struct Box
{
    Material material;
    vec3 halfSize;
    mat3 rotation;
    vec3 position;
};

struct Sphere
{
    Material material;
    vec3 position;
    float radius;
};

uniform vec2 uViewportSize;
uniform vec3 uPosition;
uniform vec3 uDirection;
uniform vec3 uUp;
uniform float uFOV;
uniform float uTime;
uniform int uSamples;

#define PI 3.1415926535
#define HALF_PI (PI / 2.0)
#define FAR_DISTANCE 1000000.0

float RandomNoise(vec2 co)
{
    co *= fract(uTime * 12.343);
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 RandomSpherePoint(vec2 rand)
{
    float cosTheta = sqrt(1.0 - rand.x);
    float sinTheta = sqrt(rand.x);
    float phi = 2.0 * PI * rand.y;
    return vec3(
        cos(phi) * sinTheta,
        sin(phi) * sinTheta,
        cosTheta
    );
}

vec3 RandomHemispherePoint(vec2 rand, vec3 n)
{
    vec3 v = RandomSpherePoint(rand);
    return dot(v, n) < 0.0 ? -v : v;
}

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
    vec3 ro = box.rotation * (origin - box.position);

    vec3 m = vec3(1.0) / rd; 

    vec3 s = vec3((rd.x < 0.0) ? 1.0 : -1.0,
                  (rd.y < 0.0) ? 1.0 : -1.0,
                  (rd.z < 0.0) ? 1.0 : -1.0);
    vec3 t1 = m * (-ro + s * box.halfSize);
    vec3 t2 = m * (-ro - s * box.halfSize);

    float tN = max(max(t1.x, t1.y), t1.z);
    float tF = min(min(t2.x, t2.y), t2.z);

    if (tN > tF || tF < 0.0) return false;

    mat3 txi = box.rotation;

    if (t1.x > t1.y && t1.x > t1.z)
        normal = txi * vec3(s.x, 0.0, 0.0);
    else if (t1.y > t1.z)
        normal = txi * vec3(0.0, s.y, 0.0);
    else
        normal = txi * vec3(0.0, 0.0, s.z);

    fraction = tN;

    return true;
}

const int SphereCount = 3;
Sphere spheres[SphereCount];

const int BoxCount = 6;
Box boxes[BoxCount];

bool CastRay(vec3 rayOrigin, vec3 rayDirection, out float fraction, out vec3 normal, out Material material)
{
    float minDistance = FAR_DISTANCE;

    for (int i = 0; i < SphereCount; i++)
    {
        float F;
        vec3 N;
        if (IntersectRaySphere(rayOrigin, rayDirection, spheres[i], F, N) && F < minDistance)
        {
            minDistance = F;
            normal = N;
            material = spheres[i].material;
        }
    }

    for (int i = 0; i < BoxCount; i++)
    {
        float F;
        vec3 N;
        if (IntersectRayBox(rayOrigin, rayDirection, boxes[i], F, N) && F < minDistance)
        {
            minDistance = F;
            normal = N;
            material = boxes[i].material;
        }
    }

    fraction = minDistance;
    return minDistance != FAR_DISTANCE;
}

#define MAX_DEPTH 6

vec3 TracePath(vec3 rayOrigin, vec3 rayDirection, float seed)
{
    vec3 L = vec3(0.0);
    vec3 F = vec3(1.0);
    for (int i = 0; i < MAX_DEPTH; i++)
    {
        float fraction;
        vec3 normal;
        Material material;
        bool hit = CastRay(rayOrigin, rayDirection, fraction, normal, material);
        if (hit)
        {
            vec3 newRayOrigin = rayOrigin + fraction * rayDirection;

            vec3 newRayDirection = vec3(0.0);
            if (material.type == MATERIAL_DIFFUSE)
            {
                vec2 rand = vec2(RandomNoise(seed * TexCoord.xy), seed * RandomNoise(TexCoord.yx));
                newRayDirection = RandomHemispherePoint(rand.xy, normal);

                vec3 randomVec = vec3(
                    RandomNoise(sin(seed * TexCoord.xy)),
                    RandomNoise(cos(seed * TexCoord.xy)),
                    RandomNoise(sin(seed * TexCoord.yx))
                );
                randomVec = 2.0 * randomVec - 1.0;

                vec3 tangent = cross(randomVec, normal);
                vec3 bitangent = cross(normal, tangent);
                mat3 tr = mat3(tangent, bitangent, normal);
                
                newRayDirection = tr * newRayDirection;
            }
            else if (material.type == MATERIAL_SPECULAR)
            {
                newRayDirection = reflect(rayDirection, normal);
            }

            rayDirection = newRayDirection;
            rayOrigin = newRayOrigin + normal * 0.8;

            L += F * material.emmitance;
            F *= material.reflectance;
        }
        else
        {
            F = vec3(0.0);
        }
    }

    return L;
}

void main()
{
    spheres[0].position = vec3(2.0, -2.0, 0.0);
    spheres[1].position = vec3(-2.5, -1.5, -2.0);
    spheres[2].position = vec3(3.0, 0.0, 2.0);
    spheres[0].radius = 1.5;
    spheres[1].radius = 2.0;
    spheres[2].radius = 1.0;
    spheres[0].material.type = MATERIAL_SPECULAR;
    spheres[1].material.type = MATERIAL_SPECULAR;
    spheres[2].material.type = MATERIAL_SPECULAR;
    spheres[0].material.reflectance = vec3(1.0, 1.0, 1.0);
    spheres[1].material.reflectance = vec3(1.0, 0.0, 0.0);
    spheres[2].material.reflectance = vec3(0.9, 0.7, 0.0);
    spheres[0].material.emmitance = vec3(0.0);
    spheres[1].material.emmitance = vec3(0.0);
    spheres[2].material.emmitance = vec3(0.0);

    boxes[0].material.type = MATERIAL_DIFFUSE;
    boxes[0].material.emmitance = vec3(0.0);
    boxes[0].material.reflectance = vec3(1.0, 1.0, 1.0);
    boxes[0].halfSize = vec3(5.0, 0.2, 5.0);
    boxes[0].position = vec3(0.0, 5.2, 0.0);
    boxes[0].rotation = mat3(
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0
    );

    boxes[1].material.type = MATERIAL_DIFFUSE;
    boxes[1].material.emmitance = vec3(0.0);
    boxes[1].material.reflectance = vec3(1.0, 1.0, 1.0);
    boxes[1].halfSize = vec3(5.0, 0.2, 5.0);
    boxes[1].position = vec3(0.0, -5.2, 0.0);
    boxes[1].rotation = mat3(
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0
    );

    boxes[2].material.type = MATERIAL_DIFFUSE;
    boxes[2].material.emmitance = vec3(0.0);
    boxes[2].material.reflectance = vec3(1.0, 0.0, 0.0);
    boxes[2].halfSize = vec3(5.0, 0.2, 5.0);
    boxes[2].position = vec3(-5.2, 0.0, 0.0);
    boxes[2].rotation = mat3(
        0.0, 1.0, 0.0,
        -1.0, 0.0, 0.0,
        0.0, 0.0, 1.0
    );

    boxes[3].material.type = MATERIAL_DIFFUSE;
    boxes[3].material.emmitance = vec3(0.0);
    boxes[3].material.reflectance = vec3(0.0, 1.0, 0.0);
    boxes[3].halfSize = vec3(5.0, 0.2, 5.0);
    boxes[3].position = vec3(5.2, 0.0, 0.0);
    boxes[3].rotation = mat3(
        0.0, 1.0, 0.0,
        -1.0, 0.0, 0.0,
        0.0, 0.0, 1.0
    );

    boxes[4].material.type = MATERIAL_DIFFUSE;
    boxes[4].material.emmitance = vec3(0.0);
    boxes[4].material.reflectance = vec3(1.0, 1.0, 1.0);
    boxes[4].halfSize = vec3(5.0, 0.2, 5.0);
    boxes[4].position = vec3(0.0, 0.0, -5.2);
    boxes[4].rotation = mat3(
        1.0, 0.0, 0.0,
        0.0, 0.0, 1.0,
        0.0, 1.0, 0.0
    );

    // light source
    boxes[5].material.type = MATERIAL_DIFFUSE;
    boxes[5].material.emmitance = vec3(10.0);
    boxes[5].material.reflectance = vec3(1.0);
    boxes[5].halfSize = vec3(2.5, 0.2, 2.5);
    boxes[5].position = vec3(0.0, 5.0, 0.0);
    boxes[5].rotation = mat3(
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0
    );

    vec3 direction = GetRayDirection(TexCoord, uViewportSize, uFOV, uDirection, uUp);

    vec3 totalColor = vec3(0.0);
    for (int i = 0; i < uSamples; i++)
    {
        float seed = sin(float(i) * uTime);
        vec3 sampleColor = TracePath(uPosition, direction, seed);
        totalColor += sampleColor;
    }

    vec3 outputColor = totalColor / float(uSamples);
    OutColor = vec4(outputColor, 1.0);
}