layout(triangles) in;
layout(triangle_strip, max_vertices = 9) out;

in vec2 TexCoord[];

out vec4 FragPos;
out vec2 FragmentTexCoord;

const int TextureSplitCount = 3;
uniform mat4 LightProjMatrix[TextureSplitCount];

void emitFace(mat4 lightMatrix, int index)
{
    for (int triangleIndex = 0; triangleIndex < 3; triangleIndex++)
    {
        FragPos = gl_in[triangleIndex].gl_Position;
        gl_Position = lightMatrix * FragPos;
        gl_Position.x = (gl_Position.x + index) / TextureSplitCount;
        EmitVertex();
    }
    EndPrimitive();
}

void main()
{
    FragmentTexCoord = TexCoord[0];

    emitFace(LightProjMatrix[0], 0);
    emitFace(LightProjMatrix[1], 1);
    emitFace(LightProjMatrix[2], 2);
}