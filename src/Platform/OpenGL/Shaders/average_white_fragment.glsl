EMBEDDED_SHADER(

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D tex;

vec3 liminance = vec3(0.2125f, 0.7154f, 0.0721f);

void main()
{
    vec3 color = texture(tex, TexCoord).rgb;
    float white = dot(liminance, color);
    OutColor = vec4(white, white, white, 1.0f);
}

)