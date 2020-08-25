EMBEDDED_SHADER(

in vec2 TexCoord;

out vec4 Color;

uniform sampler2D albedoTex;
uniform sampler2D materialTex;
uniform float weight;

void main()
{
    vec3 albedo = texture(albedoTex, TexCoord).rgb;
    float emmision = texture(materialTex, TexCoord).r;
    emmision = emmision / (1.0f - emmision);
    Color = vec4(weight * emmision * albedo, 1.0f);
}

)