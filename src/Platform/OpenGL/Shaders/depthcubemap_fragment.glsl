#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

in vec4 FragPos;

uniform vec3 lightPos;
uniform float zFar;

void main()
{
    float lightDistance = length(FragPos.xyz - lightPos);
    lightDistance = lightDistance / zFar;
    gl_FragDepth = lightDistance;
}

)