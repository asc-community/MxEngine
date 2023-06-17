#include "Library/shader_utils.glsl"
layout(binding = 0) uniform sampler2D lensFlareColor;
layout(binding = 1) uniform sampler2D inputColor;

uniform int ghosts; // number of ghost samples
uniform float ghostDispersal; // dispersion factor
uniform float uHaloWidth;

in vec2 TexCoord;
out vec4 outColor;

const float gWeight = 80.0f;//relative to pixel pos

void main()
{
	vec2 texcoord = vec2(1.0) - TexCoord;
	// ghost vector to image centre:
	vec2 ghostVec = (vec2(0.5) - texcoord) * ghostDispersal;

	// sample ghosts
	vec4 result = vec4(0.0);
	for (int i = 0; i < ghosts; ++i) {
		vec2 offset = fract(texcoord + ghostVec * float(i));
		float weight = length(vec2(0.5) - offset) / length(vec2(0.5));
		weight = pow(1.0 - weight, gWeight);

		result += texture(lensFlareColor, offset) * weight;
	}

	vec2 haloVec = normalize(ghostVec) * uHaloWidth;
	float weight = length(vec2(0.5) - fract(texcoord + haloVec)) / length(vec2(0.5));
	weight = pow(1.0 - weight, gWeight);
	vec4 haloColor = texture(inputColor, texcoord + haloVec) * weight;
	haloColor = max(calcLuminance(haloColor.rgb)-0.7f,0.0) * haloColor;
	result+=haloColor;
	outColor = result + texture(inputColor, TexCoord);
}