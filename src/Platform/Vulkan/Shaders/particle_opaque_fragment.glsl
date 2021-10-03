in vec2 TexCoord;
in vec3 WorldPosition;

layout(location = 0) out vec4 OutAlbedo;
layout(location = 1) out vec4 OutNormal;
layout(location = 2) out vec4 OutMaterial;

uniform vec3 cameraPosition;
uniform float emmision;
uniform float transparency;
uniform float metallness;
uniform float roughness;
uniform vec3 color;
uniform float gamma;
uniform sampler2D albedoTex;
uniform vec3 normal;

void main()
{
    vec4 albedo = texture(albedoTex, TexCoord).rgba;
    float alphaCutoff = 1.0 - transparency;
    if (albedo.a <= alphaCutoff) discard; // mask fragments with opacity less than cutoff

    OutAlbedo = vec4(color * pow(albedo.rgb, vec3(gamma)), emmision / (emmision + 1.0));
    OutNormal = vec4(0.5 * normal + 0.5, 1.0);
    OutMaterial = vec4(1.0, roughness, metallness, 1.0);
}