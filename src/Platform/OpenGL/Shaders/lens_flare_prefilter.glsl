#include "Library/shader_utils.glsl"

layout(binding = 0) uniform sampler2D inputColor;

uniform float uScale;
uniform float uBias;
uniform float uAverage;

in vec2 TexCoord;

out vec4 outputColor;

void main()
{
   vec3 col = texture(inputColor, TexCoord).rgb;

   float lum = calcLuminance(col) - uAverage;
   if (lum <= 0)
   {
      outputColor = vec4(vec3(0.0f), 1.0f);
      return;
   }

   lum += uBias;
   col = max(lum, 0.0f) * col * uScale;
   outputColor = vec4(col, 1.0f);
}