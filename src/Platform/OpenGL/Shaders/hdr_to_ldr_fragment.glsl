EMBEDDED_SHADER(

out vec4 OutColor;
in vec2 TexCoord;

uniform sampler2D HDRTex;
uniform sampler2D averageWhiteTex;
uniform float gamma;
uniform float colorMultiplier;
uniform float whitePoint;
uniform float exposure;
uniform vec3 ABCcoefsACES;
uniform vec3 DEFcoefsACES;

vec3 curveACES(float A, float B, float C, float D, float E, float F, vec3 color)
{
    return ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
}

vec3 toneMapACES(float A, float B, float C, float D, float E, float F, vec3 color, float numMultiplier)
{
    vec3 numerator = curveACES(A, B, C, D, E, F, color);
    numerator = max(numerator, vec3(0.0f));
    numerator *= numMultiplier;

    vec3 denominator = curveACES(A, B, C, D, E, F, vec3(11.2f));
    denominator = max(denominator, vec3(0.0f));

    return numerator / denominator;
}

void main()
{
	vec3 HDRColor = texture(HDRTex, TexCoord).rgb;
	float avgLuminance = texture(averageWhiteTex, vec2(0.0f)).r;

    avgLuminance = max(avgLuminance, 0.01f);

    float scaledWhitePoint = whitePoint * 11.2f;
    float luma = avgLuminance / scaledWhitePoint;
    luma = pow(luma, exposure);
    luma = luma * scaledWhitePoint;
    luma = whitePoint / luma;

    vec3 LDRColor = toneMapACES(
        ABCcoefsACES.x, ABCcoefsACES.y, ABCcoefsACES.z, 
        DEFcoefsACES.x, DEFcoefsACES.y, DEFcoefsACES.z, 
        luma * HDRColor, colorMultiplier);

	vec3 gammaCorrectedColor = pow(LDRColor.rgb, vec3(1.0f / gamma));
	OutColor = vec4(gammaCorrectedColor.rgb, 1.0f);
}

)