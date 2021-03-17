layout(location = 0) in vec3 position;

uniform mat4 StaticViewProjection;
uniform mat3 Rotation;

out vec3 TexCoords;

void main()
{
    TexCoords = position;
    vec3 pos = Rotation * position * 10000.0f;
    vec4 screenPos = StaticViewProjection * vec4(pos, 1.0);
    gl_Position = screenPos;
}