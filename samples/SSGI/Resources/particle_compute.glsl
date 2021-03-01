layout(local_size_x = 256) in;

uniform float dt;

layout(std430, binding = 0) buffer Position
{
    vec2 position[];
};

layout(std430, binding = 1) buffer Velocity
{
    vec2 velocity[];
};

#define PI 3.1415926535

#define PARTICLE_POSITION_SIZE_X (16 * 32)
#define PARTICLE_POSITION_SIZE_Y (16 * 32)
#define PARTICLE_POSITION_SIZE (PARTICLE_POSITION_SIZE_X * PARTICLE_POSITION_SIZE_Y)

void main()
{
    uint idx = gl_GlobalInvocationID.x;
    vec2 pos = position[idx];
    vec2 vel = velocity[idx];

    vec2 screenPos = vec2(0.0);
    screenPos.x = float(idx / PARTICLE_POSITION_SIZE_X) / PARTICLE_POSITION_SIZE_Y;
    screenPos.y = float(idx % PARTICLE_POSITION_SIZE_X) / PARTICLE_POSITION_SIZE_Y;

    vel = velocity[idx];
    if (pos.x < 0.0 || pos.x > 1.0)
        vel.x = -clamp(vel.x, -1.0, 1.0);
    if (pos.y < 0.0 || pos.y > 1.0)
        vel.y = -clamp(vel.y, -1.0, 1.0);
    
    velocity[idx] = vel;
    position[idx] = pos + vel * dt;

    //velocity[idx] = vec2(0.0);
    //position[idx] = vec2(0.0);
}