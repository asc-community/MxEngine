layout(triangles) in;
layout(triangle_strip, max_vertices = 9) out;

in vec2 VertexTexCoord[];

out vec4 FragPos;
out vec2 TexCoord;

const int TextureSplitCount = 3;
uniform mat4 LightProjMatrix[TextureSplitCount];

void emitFace(mat4 lightMatrix, int index)
{
    for (int triangleIndex = 0; triangleIndex < 3; triangleIndex++)
    {
        vec4 VertexPos = gl_in[triangleIndex].gl_Position;
        vec4 ProjPos = lightMatrix * VertexPos;
        gl_Position = ProjPos;
        gl_Position.x = gl_Position.x / TextureSplitCount + (index - 1) * 2.0 / TextureSplitCount;
        
        gl_ClipDistance[0] = 0.5 * ProjPos.x + 0.5;
        gl_ClipDistance[1] = 0.5 * -ProjPos.x + 0.5;
        EmitVertex();
    }
    EndPrimitive();
}

void main()
{
    TexCoord = VertexTexCoord[0];

    emitFace(LightProjMatrix[0], 0);
    emitFace(LightProjMatrix[1], 1);
    emitFace(LightProjMatrix[2], 2);
}