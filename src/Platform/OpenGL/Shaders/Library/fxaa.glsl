/*
Basic FXAA implementation based on the code on geeks3d.com with the
modification that the texture2DLod stuff was removed since it's
unsupported by WebGL.
--
From:
https://github.com/mitsuhiko/webgl-meincraft
Copyright (c) 2011 by Armin Ronacher.
Some rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * The names of the contributors may not be used to endorse or
      promote products derived from this software without specific
      prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// this file was modified by @MomoDeve for MxEngine project (https://github.com/asc-community/MxEngine)

const float FXAA_REDUCE_MIN = 1.0f / 128.0f;
const float FXAA_REDUCE_MUL = 1.0f / 8.0f;
const float FXAA_SPAN_MAX = 8.0f;

vec4 fxaa(sampler2D tex, vec2 texCoord)
{
    vec2 inverseVP = vec2(1.0f) / textureSize(tex, 0);
    vec2 v_rgbNW = texCoord + vec2(-1.0f, -1.0f) * inverseVP;
    vec2 v_rgbNE = texCoord + vec2( 1.0f, -1.0f) * inverseVP;
    vec2 v_rgbSW = texCoord + vec2(-1.0f,  1.0f) * inverseVP;
    vec2 v_rgbSE = texCoord + vec2( 1.0f,  1.0f) * inverseVP;
    vec2 v_rgbM  = texCoord;

    vec4 color;
    vec3 rgbNW    = texture(tex, v_rgbNW).rgb;
    vec3 rgbNE    = texture(tex, v_rgbNE).rgb;
    vec3 rgbSW    = texture(tex, v_rgbSW).rgb;
    vec3 rgbSE    = texture(tex, v_rgbSE).rgb;
    vec4 texColor = texture(tex, v_rgbM).rgba;
    vec3 rgbM = texColor.rgb;
    vec3 luma = vec3(0.299f, 0.587f, 0.114f);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25f * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);

    float rcpDirMin = 1.0f / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(vec2( FXAA_SPAN_MAX,  FXAA_SPAN_MAX),
          max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
          dir * rcpDirMin)) * inverseVP;

    vec3 rgbA = 0.5f * (
        texture(tex, texCoord + dir * (1.0f / 3.0f - 0.5f)).rgb +
        texture(tex, texCoord + dir * (2.0f / 3.0f - 0.5f)).rgb
    );
    vec3 rgbB = rgbA * 0.5f + 0.25f * (
        texture(tex, texCoord + dir * -0.5f).rgb +
        texture(tex, texCoord + dir *  0.5f).rgb
    );

    float lumaB = dot(rgbB, luma);
    if ((lumaB < lumaMin) || (lumaB > lumaMax))
        color = vec4(rgbA, texColor.a);
    else
        color = vec4(rgbB, texColor.a);
    return color;
}