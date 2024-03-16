#include "Library/fragment_utils.glsl"

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;
uniform sampler2D ssrMask;
uniform sampler2D HDRTex;
uniform sampler2D hiZTex0;
uniform sampler2D hiZTex1;

uniform Camera camera;
uniform EnvironmentInfo environment;

uniform float thickness;
uniform vec2 screenResolution;
uniform float intensity;//For users who wanna emphasize emotion 

float sampleDepth(ivec2 uv, int level)
{
    switch (level)
    {
    case 0:return texelFetch(depthTex, uv, 0).r;
    case 1:return texelFetch(hiZTex0, uv / ivec2(2), 0).r;
    case 2:return texelFetch(hiZTex1, uv / ivec2(4), 0).r;
        //Add more levels if needed
    }
}

vec3 transform(mat4 m, vec3 p)
{
    vec4 temp = m * vec4(p, 1.0);
    return temp.xyz;
}

vec4 transform(mat4 m, vec4 p)
{
    return m * p;
}

void swapComp(inout vec2 v)
{
    v = v.yx;
}

#define TRACE_RAY(stp) \
	do{\
		scr0+=dScr*int(stp);\
		vpProj0.z+=dVpProj.z*int(stp);\
		w0+=dw*int(stp);\
	}while(false);

void main()
{
	if(texelFetch(ssrMask,ivec2(gl_FragCoord.xy),0).r<0.5)
	{
		OutColor = vec4(0.0);
		return;
	}
    FragmentInfo fragment = getFragmentInfo(
        TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewProjMatrix);
    float rayLength = 8000.f;

    vec3 viewDistance = camera.position - fragment.position;
    vec3 viewDirection = normalize(viewDistance);
    vec3 reflectDirection = normalize(reflect(-viewDirection, fragment.normal));

    //0:start 1:end
    vec3 wp0 = fragment.position;
    vec3 wp1 = fragment.position + reflectDirection * rayLength;

    vec3 vp0 = transform(camera.viewMatrix, wp0);
    vec3 vp1 = transform(camera.viewMatrix, wp1);

    vec4 ho0 = transform(camera.projectionMatrix, vec4(vp0, 1.0));
    vec4 ho1 = transform(camera.projectionMatrix, vec4(vp1, 1.0));

    float w0 = 1.0 / ho0.w;
    float w1 = 1.0 / ho1.w;          
    //to avoid nonlinearity,project to view space
    vec3 vpProj0 = vp0 * w0;
    vec3 vpProj1 = vp1 * w1;

    vec2 ndc0 = ho0.xy * w0;
    vec2 ndc1 = ho1.xy * w1;

    vec2 scr0 = (ndc0 + 1.0) / 2.0 * screenResolution;
    vec2 scr1 = (ndc1 + 1.0) / 2.0 * screenResolution;

    scr1 += distance(scr0, scr1) < 0.01 ? 0.01 : 0.0;
    //Using Line Generation Algorithm
    //https://en.wikipedia.org/wiki/Digital_differential_analyzer_(graphics_algorithm)
    vec2 delta = scr1 - scr0;
    bool permute = false;
    if (abs(delta.x) < abs(delta.y))
    {
        permute = true;
        swapComp(delta);
        swapComp(scr0);
        swapComp(scr1);
    }
    float stepDir = sign(delta.x);
    float invDx = stepDir / delta.x;

    vec3 dVpProj = (vpProj1 - vpProj0) * invDx;
    float dw = (w1 - w0) * invDx;
    vec2 dScr = vec2(stepDir, delta.y * invDx);
    
    //Start position
    scr0 += dScr;
    vpProj0 += dVpProj;
    w0 += dw;

    float lastZmax = vp0.z;
    vec2 result;
    bool isHit = false;
    bool checkthickness = false;
    int level = 0;
    int maxLevel = 2;
    float testStp = 0.0;
    int maxStp = int(sqrt(screenResolution.x * screenResolution.x + screenResolution.y * screenResolution.y));
    //Todo: /(ㄒoㄒ)/~~ Handle diffuse lobe. Sampling more rays or using cone tracing when the BRDF lobe is fat.
    for (int curStep = 0; curStep < maxStp; curStep++, testStp++)
    {
        //reconstruct camera space ray depth
        result.xy = permute ? scr0.yx : scr0;
        vec2 depths;
        depths.x = lastZmax;
        depths.y = (dVpProj.z * 0.5 + vpProj0.z) / (dw * 0.5 + w0);
        lastZmax = depths.y;
        //check direction
        if (depths.x < depths.y)
            swapComp(depths);
        //check boundary
        if (result.x > screenResolution.x || result.x < 0 ||
            result.y > screenResolution.y || result.y < 0)
            break;
        //reconstruct in cam space 
        float d = sampleDepth(ivec2(result), level);
        vec4 normPosition = vec4(2.0f * result - vec2(1.0f), d, 1.0f);
        vec4 vpos = transform(camera.invProjectionMatrix, normPosition);
        vpos /= vpos.w;
        float sceneDepth = vpos.z;
        //check if near the start point
        if (!checkthickness && (depths.y - sceneDepth > thickness))
            checkthickness = true;
        //check hit & switch depth layer
        float curDiff = sceneDepth - depths.y;
        if (checkthickness &&
            (curDiff > 0 || curDiff < 0 && abs(curDiff) < thickness))
        {
            if (level == 0)
            {
                isHit = true;
                break;
            }
            TRACE_RAY(-pow(2.0, level));
            level = 0;
        }
        else
        {
            level = min(maxLevel, level + 1);
            TRACE_RAY(pow(2.0, level));
        }
    }

    if (isHit)
        OutColor = vec4(texelFetch(HDRTex, ivec2(result), 0).rgb * intensity, 1.0);
    else
        OutColor = vec4(0.0);
}