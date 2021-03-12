layout(local_size_x = 64) in;

uniform float dt;
uniform float lifetime;
uniform vec3 spawnpoint;

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

void main()
{
    const uint idx = gl_GlobalInvocationID.x;

    Particle particle = particleData[idx];

    vec3 particlePosition = particle.position_timeAlive.xyz;
    float particleTimeAlive = particle.position_timeAlive.w;
    vec3 particleVelocity = particle.velocity_size.xyz;
    float particleSize = particle.velocity_size.w;
    float particleSpawnDistance = particle.none3_spawnDistance.w;

    particleTimeAlive += dt;
    particlePosition += particleVelocity * dt;

    if (particleTimeAlive > lifetime)
    {
        vec3 spawnOffset = particleSpawnDistance * normalize(particleVelocity);
        particlePosition = spawnpoint + spawnOffset;
        particleTimeAlive = 0.0;
    }

    particleData[idx] = Particle(
        vec4(particlePosition, particleTimeAlive), 
        vec4(particleVelocity, particleSize),
        vec4(vec3(0.0), particleSpawnDistance)
    );
}