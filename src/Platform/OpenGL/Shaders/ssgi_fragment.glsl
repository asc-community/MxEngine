in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D inputTex;

#define PI 3.1416926535

float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 sampleTexture(sampler2D tex, vec2 uv, vec2 origin)
{
    float v = clamp(pow(dot(uv, uv), 0.1), 0.0, 1.0);
    float lod = 8.0 * v;
    vec3 val = textureLod(tex, origin + uv, lod).rgb;
    return val;
}

void main()
{
    float r = rand(TexCoord);
    vec2 invSize = 1.0 / textureSize(inputTex, 0);
    const int SAMPLES = 20;
    vec3 accum = vec3(0.0);
    for (int i = 0; i < SAMPLES; i++)
    {
        float mMod = (i % (SAMPLES / 4) + 1) * 1.2 + r;
        float mExp = exp(mMod);
        float phi = ((i + r * SAMPLES) * 2.0 * PI) / SAMPLES;
        vec2 uv = mExp * vec2(cos(phi), sin(phi));
        uv = uv * invSize;
        accum += sampleTexture(inputTex, uv, TexCoord);
    }

    vec3 originalColor = texture(inputTex, TexCoord).rgb;

    OutColor = vec4(accum, 1.0);
}