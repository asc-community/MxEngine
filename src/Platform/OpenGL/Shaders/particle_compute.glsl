layout(local_size_x = 64) in;

uniform float dt;
uniform float lifetime;
uniform vec3 spawnpoint;

struct Particle
{
    vec4 position_timeAlive;
    vec4 velocity_size;
};

layout(std430, binding = 0) buffer ParticleData
{
    Particle particleData[];
};

void main()
{
    const uint idx = gl_GlobalInvocationID.x;

    Particle particle = particleData[idx];

    vec3 particlePosition = particle.position_timeAlive.xyz;
    float particleTimeAlive = particle.position_timeAlive.w;
    vec3 particleVelocity = particle.velocity_size.xyz;
    float particleSize = particle.velocity_size.w;

    particleTimeAlive += dt;
    particlePosition += particleVelocity * dt;

    if (particleTimeAlive > lifetime)
    {
        particlePosition = spawnpoint;
        particleTimeAlive = 0.0;
    }

    particleData[idx] = Particle(
        vec4(particlePosition, particleTimeAlive), 
        vec4(particleVelocity, particleSize)
    );
}