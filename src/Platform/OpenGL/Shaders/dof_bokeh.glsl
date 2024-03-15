#include "Library/fragment_utils.glsl"
in vec2 TexCoord;
out vec4 OutColor;
layout(binding = 0)uniform sampler2D colorTex;
layout(binding = 1)uniform sampler2D cocTex;

const int kernelSampleCount = 22;
const vec2 diskKernel[kernelSampleCount] = {
    vec2(0,0),
    vec2(0.53333336,0),
    vec2(0.3325279,0.4169768),
    vec2(-0.11867785,0.5199616),
    vec2(-0.48051673,0.2314047),
    vec2(-0.48051673,-0.23140468),
    vec2(-0.11867763,-0.51996166),
    vec2(0.33252785,-0.4169769),
    vec2(1,0),
    vec2(0.90096885,0.43388376),
    vec2(0.6234898,0.7818315),
    vec2(0.22252098,0.9749279),
    vec2(-0.22252095,0.9749279),
    vec2(-0.62349,0.7818314),
    vec2(-0.90096885,0.43388382),
    vec2(-1,0),
    vec2(-0.90096885,-0.43388376),
    vec2(-0.6234896,-0.7818316),
    vec2(-0.22252055,-0.974928),
    vec2(0.2225215,-0.9749278),
    vec2(0.6234897,-0.7818316),
    vec2(0.90096885,-0.43388376),
};

uniform float bokehRadius = 10.0;

void main()
{
    vec3 color = vec3(0.f);
    float dx = dFdx(TexCoord.x);
    float dy = dFdy(TexCoord.y);
    float weight = 0.f;

    float coc0 = texelFetch(cocTex, ivec2(gl_FragCoord.xy) + ivec2(1,1), 0).r;
    float coc1 = texelFetch(cocTex, ivec2(gl_FragCoord.xy) + ivec2(-1,-1), 0).r;
    float coc2 = texelFetch(cocTex, ivec2(gl_FragCoord.xy) + ivec2(-1,1), 0).r;
    float coc3 = texelFetch(cocTex, ivec2(gl_FragCoord.xy) + ivec2(1,-1), 0).r;

    float cocMin = min(min(min(coc0, coc1), coc2), coc3);
    float cocMax = max(max(max(coc0, coc1), coc2), coc3);
    float coc = (-cocMin >= cocMax ? cocMin : cocMax);
 
    for (int i = 0; i < kernelSampleCount; i++)
    {
        vec2 offset = diskKernel[i] * vec2(dx, dy) * coc * bokehRadius;
        color += texture(colorTex, TexCoord + offset, 0).rgb;
    }

    color *= 1.0 / kernelSampleCount;
    OutColor = vec4(color, coc);
}