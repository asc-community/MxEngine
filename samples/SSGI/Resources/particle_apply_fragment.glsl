in vec2 TexCoord;
out vec4 OutColor;

layout(std430, binding = 0) buffer ParticlePosition
{
    vec2 particlePosition[];
};

#define PARTICLE_POSITION_SIZE_X (16 * 32)
#define PARTICLE_POSITION_SIZE_Y (16 * 32)
#define PARTICLE_POSITION_SIZE PARTICLE_POSITION_SIZE_X * PARTICLE_POSITION_SIZE_Y

void main()
{
    vec2 texelPos = vec2(
        uint(TexCoord.x * (PARTICLE_POSITION_SIZE_Y - 1)),
        uint(TexCoord.y * (PARTICLE_POSITION_SIZE_X - 1))
    );
    vec2 color = particlePosition[uint(texelPos.x * PARTICLE_POSITION_SIZE_Y + texelPos.y)];
    OutColor = vec4(color, 0.0, 1.0);
}