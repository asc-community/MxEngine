#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

layout(location = 0) in vec3 position;

uniform mat4 ViewProjection;
uniform float size;

out vec3 TexCoords;

void main()
{
    TexCoords = position;
    vec3 pos = position * size;
    vec4 screenPos = ViewProjection * vec4(pos, 1.0);
    gl_Position = screenPos;
}

)