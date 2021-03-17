in vec2 TexCoord;
in vec3 WorldPosition;

layout(location = 0) out vec4 OutAlbedo;
layout(location = 1) out vec4 OutNormal;
layout(location = 2) out vec4 OutMaterial;

uniform vec3 cameraPosition;
uniform float emmision;
uniform float metallness;
uniform float roughness;
uniform vec3 color;
uniform sampler2D albedoTex;

void main()
{
    vec4 albedo = texture(albedoTex, TexCoord).rgba;
    if (albedo.a < 0.5)
        discard; // alpha mask

    vec3 normal = normalize(cameraPosition - WorldPosition);

    OutAlbedo = vec4(color * albedo.rgb, 1.0);
    OutNormal = vec4(0.5 * normal + 0.5, 1.0);
    OutMaterial = vec4(emmision / (emmision + 1.0), roughness, metallness, 1.0);
}