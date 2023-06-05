#include "Library/shader_utils.glsl"

layout(binding = 0) uniform sampler2D inputColor;

uniform float uScale;
uniform float uBias;

in vec2 TexCoord;

out vec4 outputColor;

void main()
{
   vec3 col = texture(inputColor, TexCoord).rgb;
   float lum = calcLuminance(col) + uBias;
   col = max(lum,0.0) * col * uScale;
   outputColor=vec4(col,1.0f); 
}