in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D particleTex;
uniform sampler2D vorticityTex;

uniform float vorticityScale;
uniform float timeDelta;

vec2 computeAbsGradient(sampler2D tex, vec3 texCoord)
{
    vec2 texelSize = 1.0f / textureSize(tex, 0);

    float x1 = texture(tex, texCoord + vec2( 1.0f,  0.0f) * texelSize).x;
    float x2 = texture(tex, texCoord + vec2(-1.0f,  0.0f) * texelSize).x;
    float y1 = texture(tex, texCoord + vec2( 0.0f,  1.0f) * texelSize).x;
    float y2 = texture(tex, texCoord + vec2( 0.0f, -1.0f) * texelSize).x;

    vec2 gradValue = vec2((abs(x1) - abs(x2)), (abs(y1) - abs(y2))) * 0.5f;
    return gradValue;
}

void main()
{
    vec4 particle = texture(particleTex, TexCoord);

    vec2 absGrad = computeAbsGradient(vorticityTex, TexCoord);
    absGrad.y *= -1.0f;
    
    vec2 absGradNorm = normalize(absGrad);
    float vort = texture(vorticityTex, TexCoord).x;
    vec2 vortForce = absGradNorm * vort * vorticityScale;

    OutColor = vec4(particle.xy + vortForce * timeDelta, particle.zw);
}