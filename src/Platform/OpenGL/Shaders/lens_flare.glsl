layout(binding = 0) uniform sampler2D lensFlareColor;
layout(binding = 1) uniform sampler2D inputColor;

uniform int ghosts; // number of ghost samples
uniform float ghostDispersal; // dispersion factor
uniform float uHaloWidth;

in vec2 TexCoord;
out vec4 outColor;

void main() 
{
	vec2 texcoord = TexCoord;
	texcoord = -texcoord + vec2(1.0);
    // ghost vector to image centre:
    vec2 ghostVec = (vec2(0.5) - texcoord) * ghostDispersal;

    // sample ghosts
    vec4 result = vec4(0.0);
    for (int i = 0; i < ghosts; ++i) { 
        vec2 offset = fract(texcoord + ghostVec * float(i));
        float weight = length(vec2(0.5) - offset) / length(vec2(0.5));
        weight = pow(1.0 - weight, 80.0);

        result += texture(lensFlareColor, offset)*weight;
    }

    // vec2 haloVec = normalize(ghostVec) * uHaloWidth;
    // float weight = length(vec2(0.5) - fract(texcoord + haloVec)) / length(vec2(0.5));
    // weight = pow(1.0 - weight, 5.0);
    // result += texture(inputColor, texcoord + haloVec) * weight;

    outColor = result + texture(inputColor,TexCoord);
    //float weight = length(vec2(0.5) - TexCoord)/length(vec2(0.5));
    //weight = pow(weight, 20.0);
    //outColor=vec4(weight);
    //weight = pow(1.0 - weight, 20.0);
}