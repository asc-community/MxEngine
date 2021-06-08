layout(local_size_x = 64) in;

uniform int bufferOffset;
uniform float dt;
uniform float lifetime;
uniform vec3 spawnpoint;

struct Particle
{
    vec4 position_timeAlive;
    vec4 velocity_size;
    vec4 angularParams_spawnDistance;
};

layout(std430, binding = 0) buffer ParticleData
{
    Particle particleData[];
};

void main()
{
    uint idx = bufferOffset + gl_GlobalInvocationID.x;

    Particle particle = particleData[idx];

    vec3 particlePosition = particle.position_timeAlive.xyz;
    float particleTimeAlive = particle.position_timeAlive.w;
    vec3 particleVelocity = particle.velocity_size.xyz;
    float particleSize = particle.velocity_size.w;
    vec3 angularAxis = normalize(particle.angularParams_spawnDistance.xyz);
    float angularSpeed = length(particle.angularParams_spawnDistance.xyz);
    float particleSpawnDistance = particle.angularParams_spawnDistance.w;

    particleTimeAlive += dt;
    particlePosition += particleVelocity * dt;

    vec3 radius = particlePosition - spawnpoint;
    vec3 particleAngularVelocity = cross(normalize(radius), angularAxis);
    particlePosition += angularSpeed * particleAngularVelocity * dt;

    if (particleTimeAlive > lifetime)
    {
        vec3 particleSpawnOffset = particleSpawnDistance * normalize(particleVelocity);
        vec3 particleOrigin = spawnpoint + particleSpawnOffset;
        particlePosition = particleOrigin;
        particleTimeAlive = 0.0;
    }

    particleData[idx] = Particle(
        vec4(particlePosition, particleTimeAlive), 
        vec4(particleVelocity, particleSize),
        vec4(angularSpeed * angularAxis, particleSpawnDistance)
    );
}