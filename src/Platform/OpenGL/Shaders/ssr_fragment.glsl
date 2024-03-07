#include "Library/shader_utils.glsl"

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;
uniform sampler2D HDRTex;

struct Camera
{
    vec3 position;
    mat4 viewProjMatrix;
    mat4 invViewProjMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

uniform Camera camera;
uniform EnvironmentInfo environment;

uniform int   steps;
uniform float thickness;
uniform float startDistance;

float sampleDepth(vec2 uv)
{
	return texture(depthTex,uv).r;
}

vec3 transform(mat4 m,vec3 p)
{
	vec4 temp=m*vec4(p,1.0);
	return temp.xyz;
}

vec4 transform(mat4 m,vec4 p)
{
	return m*p;
}

void swapComp(inout vec2 v)
{
	v=v.yx;
}

void main()
{
	vec2 uResolution=vec2(1600.0,900.0);
    FragmentInfo fragment = getFragmentInfo(
        TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewProjMatrix);
	float rayLength=1000.f;
	
	vec3 viewDistance = camera.position - fragment.position;
    vec3 viewDirection = normalize(viewDistance);
    vec3 reflectDirection = normalize(reflect(-viewDirection, fragment.normal));
    
    vec3 wp0 = fragment.position;
	vec3 wp1 = fragment.position+reflectDirection*rayLength;
	
	vec3 vp0=transform(camera.viewMatrix,wp0);
	vec3 vp1=transform(camera.viewMatrix,wp1);

	vec4 ho0=transform(camera.projectionMatrix,vec4(vp0,1.0));
	vec4 ho1=transform(camera.projectionMatrix,vec4(vp1,1.0));
	
	float w0=1.0/ho0.w;
	float w1=1.0/ho1.w;
	
	vec3 vpProj0=vp0*w0;
	vec3 vpProj1=vp1*w1;
	
	vec2 ndc0=ho0.xy*w0;
	vec2 ndc1=ho1.xy*w1;
	
	vec2 scr0 =(ndc0+1.0)/2*uResolution;
	vec2 scr1 =(ndc1+1.0)/2*uResolution;
	
	scr1+=distance(scr0,scr1)<0.01?0.01:0.0;
	
	vec2 delta=scr1-scr0;
	bool permute=false;
	if(abs(delta.x) < abs(delta.y))
	{
		permute=true;
		swapComp(delta);
		swapComp(scr0);
		swapComp(scr1);
	}
	float stepDir=sign(delta.x);
	float invDx=stepDir/delta.x;
	
	vec3 dVpProj=(vpProj1-vpProj0)*invDx;
	float dw=(w1-w0)*invDx;
	vec2 dScr=vec2(stepDir,delta.y*invDx);
	
	scr0+=dScr;
	vpProj0+=dVpProj;
	w0+=dw;
	
	float lastZmax = vp0.z;
	vec2 result;
	bool isHit=false;
	const mat4 invProj = inverse(camera.projectionMatrix);
	int step=0;
	bool checkthickness=false;
	for(;step<2000;step++,scr0+=dScr,vpProj0.z+=dVpProj.z,w0+=dw)
	{
		result.xy=permute?scr0.yx:scr0;
		vec2 depths;
		depths.x=lastZmax;
		depths.y=(dVpProj.z*0.5+vpProj0.z)/(dw*0.5+w0);
		lastZmax=depths.y;
		if(depths.x<depths.y)
			swapComp(depths);
		if( result.x > uResolution.x || result.x < 0 || result.y > uResolution.y || result.y < 0)
			break;
			
		vec2 uv=result/uResolution;
		float d = sampleDepth(uv);
	    vec4 normPosition = vec4(2.0f * uv - vec2(1.0f), d, 1.0f);
	    vec4 vpos = invProj * normPosition;
	    vpos/=vpos.w;
		float sceneDepth=vpos.z;

		if(!checkthickness&&(depths.y-sceneDepth>thickness))
			checkthickness=true;
		float curDiff=sceneDepth-depths.y;
		float prevDiff=sceneDepth-depths.x;
		if((curDiff>0 && prevDiff<0) ||
		   (checkthickness && abs(curDiff)<thickness && abs(prevDiff)<thickness))
		{
			isHit=true;
			break;
		}
	}
	
	if(isHit)
		OutColor = vec4(texture(HDRTex,result/uResolution).rgb*4.0,1.0);
	else
		OutColor =vec4(0.0);
}