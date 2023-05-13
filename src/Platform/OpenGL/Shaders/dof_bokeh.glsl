#include "Library/shader_utils.glsl"
in vec2 TexCoord;
out vec4 OutColor;
layout (binding = 0)uniform sampler2D colorTex;
layout (binding = 1)uniform sampler2D cocTex;

const int kernelSampleCount = 43;
const vec2 diskKernel[kernelSampleCount] = {
    vec2(0, 0),
    vec2(0.36363637, 0),
    vec2(0.22672357, 0.28430238),
    vec2(-0.08091671, 0.35451925),
    vec2(-0.32762504, 0.15777594),
    vec2(-0.32762504, -0.15777591),
    vec2(-0.08091656, -0.35451928),
    vec2(0.22672352, -0.2843024),
    vec2(0.6818182, 0),
    vec2(0.614297, 0.29582983),
    vec2(0.42510667, 0.5330669),
    vec2(0.15171885, 0.6647236),
    vec2(-0.15171883, 0.6647236),
    vec2(-0.4251068, 0.53306687),
    vec2(-0.614297, 0.29582986),
    vec2(-0.6818182, 0),
    vec2(-0.614297, -0.29582983),
    vec2(-0.42510656, -0.53306705),
    vec2(-0.15171856, -0.66472363),
    vec2(0.1517192, -0.6647235),
    vec2(0.4251066, -0.53306705),
    vec2(0.614297, -0.29582983),
    vec2(1, 0),
    vec2(0.9555728, 0.2947552),
    vec2(0.82623875, 0.5633201),
    vec2(0.6234898, 0.7818315),
    vec2(0.36534098, 0.93087375),
    vec2(0.07473, 0.9972038),
    vec2(-0.22252095, 0.9749279),
    vec2(-0.50000006, 0.8660254),
    vec2(-0.73305196, 0.6801727),
    vec2(-0.90096885, 0.43388382),
    vec2(-0.98883086, 0.14904208),
    vec2(-0.9888308, -0.14904249),
    vec2(-0.90096885, -0.43388376),
    vec2(-0.73305184, -0.6801728),
    vec2(-0.4999999, -0.86602545),
    vec2(-0.222521, -0.9749279),
    vec2(0.07473029, -0.99720377),
    vec2(0.36534148, -0.9308736),
    vec2(0.6234897, -0.7818316),
    vec2(0.8262388, -0.56332),
    vec2(0.9555729, -0.29475483),
};
uniform float bokehRadius = 10.0;
void main()
{ 
    vec3 color = vec3(0.f);
    float dx = dFdx(TexCoord.x);
    float dy = dFdy(TexCoord.y);
    float weight = 0.f;
	float coc = texelFetch(cocTex,ivec2(gl_FragCoord.xy),0).x;
	for(int i=0;i<kernelSampleCount;i++)
	{
		vec2 offset = diskKernel[i] * vec2(dx,dy)*bokehRadius*coc;
		color+=texture(colorTex,TexCoord+offset,0).rgb;
	}

    color *= 1.0 / kernelSampleCount;
    OutColor = vec4(color,coc);
}