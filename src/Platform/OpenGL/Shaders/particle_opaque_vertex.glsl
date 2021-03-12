layout(location = 0) in vec4 position;

out vec2 TexCoord;
out vec3 WorldPosition;

struct Particle
{
    vec4 position_timeAlive;
    vec4 velocity_size;
    vec4 none3_spawnDistance;
};

layout(std430, binding = 0) buffer ParticleData
{
    Particle particleData[];
};

uniform mat4 projMatrix;
uniform float aspectRatio;
uniform vec3 relativePosition;

void main()
{
    Particle particle = particleData[gl_InstanceID];
    vec3 particlePosition = particle.position_timeAlive.xyz;
    float particleSize = particle.velocity_size.w;

    vec4 projectedPosition = projMatrix * vec4(particlePosition + relativePosition, 1.0);
    projectedPosition.xy += particleSize * vec2(position.x, aspectRatio * position.y);

    gl_Position = projectedPosition;
    WorldPosition = particlePosition;
	TexCoord = position.xy * 0.5 + vec2(0.5);
}