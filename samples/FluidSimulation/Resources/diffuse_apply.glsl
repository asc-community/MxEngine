in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D particleTex;

uniform float vorticityScale;
uniform float timeDelta;
uniform float diffusionScale;

vec2 jacobiVelocity(texture tex, vec2 texCoord, vec2 B, float alpha, float beta)
{
    vec2 texelSize = 1.0f / textureSize(tex, 0);

    vec2 vU = texture(tex, texCoord + vec2( 1.0f,  0.0f) * texelSize).xy;
    vec2 vD = texture(tex, texCoord + vec2(-1.0f,  0.0f) * texelSize).xy;
    vec2 vL = texture(tex, texCoord + vec2( 0.0f,  1.0f) * texelSize).xy;
    vec2 vR = texture(tex, texCoord + vec2( 0.0f, -1.0f) * texelSize).xy;

    v = (vU + vD + vL + vR + B * alpha) * (1.0f / beta);
    return v;
}

void main()
{
    vec4 particle = texture(particleTex, TexCoord);

    float alpha = diffusionScale * diffusionScale / timeDelta;
    float beta = 4.0f + diffusionScale;

    vec2 velocity = jacobiVelocity(particleTex, TexCoord, particle.xy, alpha, beta);

    OutColor = vec4(velocity, particle.zw);
}