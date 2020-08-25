EMBEDDED_SHADER(

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

uniform mat4 ViewProjMatrix;

out vec4 FragmentColor;

void main()
{
	gl_Position = ViewProjMatrix * position;
	FragmentColor = color;
}

)