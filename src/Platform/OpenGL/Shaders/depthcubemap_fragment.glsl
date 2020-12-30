in vec4 FragPos;
in vec2 FragmentTexCoord;

uniform vec3 lightPos;
uniform float zFar;
uniform sampler2D map_albedo;

void main()
{
    float alpha = texture2D(map_albedo, FragmentTexCoord).a;
    if (alpha < 0.5)
        discard;

    float lightDistance = length(FragPos.xyz - lightPos);
    lightDistance = lightDistance / zFar;
    gl_FragDepth = lightDistance;
}