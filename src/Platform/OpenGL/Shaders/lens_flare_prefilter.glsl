layout(binding = 0) uniform sampler2D inputColor;

uniform float uScale;
uniform float uBias;

in vec2 TexCoord;

out vec4 outputColor;

void main()
{
   vec4 col = texture(inputColor, TexCoord);
   if(length(max(vec4(0.0),col+vec4(uBias)))<=0.001f)
      outputColor=vec4(0.0);
   else
      outputColor=col*vec4(uScale);
}