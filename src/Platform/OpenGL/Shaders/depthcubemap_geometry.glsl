layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

in vec2 TexCoord[];

out vec4 FragPos;
out vec2 FragmentTexCoord;

uniform mat4 LightProjMatrix[6];

void emitFace(mat4 lightMatrix)
{
    for (int triangleIndex = 0; triangleIndex < 3; triangleIndex++)
    {
        FragPos = gl_in[triangleIndex].gl_Position;
        gl_Position = lightMatrix * FragPos;
        EmitVertex();
    }
    EndPrimitive();
}

void main()
{
    FragmentTexCoord = TexCoord[0];
    // gl_Layer must be assigned to a constant to work on most devices
    gl_Layer = 0;
    emitFace(LightProjMatrix[0]);

    gl_Layer = 1;
    emitFace(LightProjMatrix[1]);

    gl_Layer = 2;
    emitFace(LightProjMatrix[2]);

    gl_Layer = 3;
    emitFace(LightProjMatrix[3]);

    gl_Layer = 4;
    emitFace(LightProjMatrix[4]);

    gl_Layer = 5;
    emitFace(LightProjMatrix[5]);
}