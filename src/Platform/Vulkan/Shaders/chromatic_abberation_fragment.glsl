in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D tex;

uniform vec3 chromaticAbberationParams;

vec3 applyChromaticAbberation(vec3 color, vec2 texcoord, sampler2D inputTex, float minDistance, float intensity, float distortion)
{
    vec2 chromaticAberrationOffset = 2.0f * texcoord - 1.0f;
    float chromaticAberrationOffsetLength = length(chromaticAberrationOffset);
    chromaticAberrationOffsetLength *= distortion;
    float chromaticAberrationTexel = chromaticAberrationOffsetLength - minDistance;

    bool applyChromaticAberration = chromaticAberrationTexel > 0.0f;
    if (applyChromaticAberration)
    {
        chromaticAberrationTexel *= chromaticAberrationTexel;
        chromaticAberrationOffsetLength = max(chromaticAberrationOffsetLength, 0.0001f);

        float multiplier = chromaticAberrationTexel / chromaticAberrationOffsetLength;

        chromaticAberrationOffset *= multiplier * intensity;

        vec2 offsetUV = texcoord - 2.0f * chromaticAberrationOffset;
        color.r = texture(inputTex, offsetUV).r;

        offsetUV = texcoord - chromaticAberrationOffset;
        color.g = texture(inputTex, offsetUV).g;
    }
    return color;
}

void main()
{
    vec3 inputColor   = texture(tex, TexCoord).rgb;
    float minDistance = chromaticAbberationParams.x;
    float intensity   = chromaticAbberationParams.y;
    float distortion  = chromaticAbberationParams.z;
    vec3 outputColor  = applyChromaticAbberation(inputColor, TexCoord, tex, minDistance, intensity, distortion);
    OutColor = vec4(outputColor, 1.0f);
}