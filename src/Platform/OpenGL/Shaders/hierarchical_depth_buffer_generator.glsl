layout(binding = 0) uniform sampler2D depthBuffer;
uniform ivec2 uPreviousLevelRes;

out vec4 OutColor;

void main()
{
    ivec2 currentTexCoord = ivec2(gl_FragCoord);
    ivec2 lastTexCoord = 2 * currentTexCoord;

    vec4 depth;
    depth.x = texelFetch(depthBuffer,lastTexCoord, 0).r;
    depth.y = texelFetch(depthBuffer, lastTexCoord + ivec2(1, 0), 0).r;
    depth.z = texelFetch(depthBuffer,lastTexCoord + ivec2(1, 1),0).r;
    depth.w = texelFetch(depthBuffer,lastTexCoord + ivec2(0, 1), 0).r;

    float minDepth = min(min(depth.x, depth.y),
                         min(depth.z, depth.w));
        
    bool extraCol = ((uPreviousLevelRes.x & 1) != 0);
    bool extraRow = ((uPreviousLevelRes.y & 1) != 0);
    if (extraCol) 
    {
        vec2 col;
        col.x = texelFetch(depthBuffer, lastTexCoord + ivec2(2, 0), 0).r;
        col.y = texelFetch(depthBuffer, lastTexCoord + ivec2(2, 1), 0).r;
            
        if (extraRow) 
        {
            float corner = texelFetch(depthBuffer, lastTexCoord + ivec2(2, 2), 0).r;
            minDepth = min(minDepth, corner);
        }
        minDepth = min(minDepth, min(col.x, col.y));
    }
    if (extraRow) 
    {
        vec2 row;
        row.x = texelFetch(depthBuffer, lastTexCoord + ivec2(0, 2), 0).r;
        row.y = texelFetch(depthBuffer, lastTexCoord + ivec2(1, 2), 0).r;
        minDepth = min(minDepth, min(row.x, row.y));
    }

    OutColor = vec4(minDepth,0.f,0.f,1.f);
}
