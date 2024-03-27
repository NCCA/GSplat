#version 410 core


out vec4 pointColour;
uniform mat4 view;
uniform mat4 projection;
uniform samplerBuffer posSampler;
uniform samplerBuffer colourSampler;
uniform samplerBuffer scaleSampler;
uniform samplerBuffer rotationSampler;
out mat3 rotation;
mat3 quatToMat(vec4 q) {
    return mat3(2.0 * (q.x * q.x + q.y * q.y) - 1.0, 2.0 * (q.y * q.z + q.x * q.w), 2.0 * (q.y * q.w - q.x * q.z), // 1st column
                2.0 * (q.y * q.z - q.x * q.w), 2.0 * (q.x * q.x + q.z * q.z) - 1.0, 2.0 * (q.z * q.w + q.x * q.y), // 2nd column
                2.0 * (q.y * q.w + q.x * q.z), 2.0 * (q.z * q.w - q.x * q.y), 2.0 * (q.x * q.x + q.w * q.w) - 1.0); // last column
}

void main()
{
    vec3 inPos=texelFetch(posSampler,gl_VertexID).xyz;
    vec4 inColour=texelFetch(colourSampler,gl_VertexID);

    vec3 scale=texelFetch(scaleSampler,gl_VertexID).xyz;
    vec3 scaled=scale*inPos;
    rotation=quatToMat(texelFetch(rotationSampler,gl_VertexID));

    gl_Position = vec4(inPos,1.0f);
    pointColour.rgb = inColour.rgb * 0.28+vec3(0.5,0.5,0.5);
    pointColour.a = 0.5;//inColour.a;
}
