EMBEDDED_SHADER(

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