in vec2 TexCoord;
in vec3 WorldPosition;

layout(location = 0) out vec4 OutAlbedo;
layout(location = 1) out vec4 OutNormal;
layout(location = 2) out vec4 OutMaterial;

uniform vec3 cameraPosition;

void main()
{
    float emmision = 1.0;
    float roughness = 0.0;
    float metallic = 0.0;
    vec3 normal = normalize(cameraPosition - WorldPosition);

    OutAlbedo = vec4(1.0, 0.3, 0.0, 1.0);
    OutNormal = vec4(0.5 * normal + 0.5, 1.0f);
    OutMaterial = vec4(emmision / (emmision + 1.0f), roughness, metallic, 1.0f);
}