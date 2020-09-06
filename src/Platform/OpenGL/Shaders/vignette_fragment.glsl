EMBEDDED_SHADER(

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D tex;
uniform float radius;
uniform float intensity;
uniform vec2 viewportSize;

vec3 applyVignette(vec3 color, vec2 texcoord, float radius, float intensity)
{
    texcoord *= 1.0f - texcoord.yx;
    float vig = texcoord.x * texcoord.y * intensity;
    vig = pow(vig, radius);
    return color * min(vig, 1.0f);
}

void main()
{
    vec3 color = texture(tex, TexCoord).rgb;
    color = applyVignette(color, TexCoord, radius, intensity);
    OutColor = vec4(color, 1.0f);
}

)