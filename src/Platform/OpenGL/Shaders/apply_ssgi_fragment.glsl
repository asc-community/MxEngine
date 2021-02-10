in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D inputTex;
uniform sampler2D SSGITex;

float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    float r = rand(TexCoord) + 1.0;
    vec3 originalColor = texture(inputTex, TexCoord).rgb;

    int LOD = 2;
    vec2 invSizeLOD = 1.0 / textureSize(SSGITex, LOD);

    const int SAMPLES = 8;
    vec3 ssgi[SAMPLES];
    ssgi[0] = textureLod(SSGITex, TexCoord + r * vec2( 0.0,  1.0) * invSizeLOD, LOD).rgb;
    ssgi[1] = textureLod(SSGITex, TexCoord + r * vec2( 1.0,  0.0) * invSizeLOD, LOD).rgb;
    ssgi[2] = textureLod(SSGITex, TexCoord + r * vec2( 0.0, -1.0) * invSizeLOD, LOD).rgb;
    ssgi[3] = textureLod(SSGITex, TexCoord + r * vec2(-1.0,  0.0) * invSizeLOD, LOD).rgb;
    ssgi[4] = textureLod(SSGITex, TexCoord + r * vec2( 1.0,  1.0) * invSizeLOD, LOD).rgb;
    ssgi[5] = textureLod(SSGITex, TexCoord + r * vec2(-1.0,  1.0) * invSizeLOD, LOD).rgb;
    ssgi[6] = textureLod(SSGITex, TexCoord + r * vec2(-1.0, -1.0) * invSizeLOD, LOD).rgb;
    ssgi[7] = textureLod(SSGITex, TexCoord + r * vec2( 1.0, -1.0) * invSizeLOD, LOD).rgb;
    
    vec3 total = vec3(0.0);
    for (int i = 0; i < SAMPLES; i++)
        total += ssgi[i];
    vec3 ssgiColor = total / SAMPLES;

    OutColor = vec4(originalColor + ssgiColor, 1.0);
}