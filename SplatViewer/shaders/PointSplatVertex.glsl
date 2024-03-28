#version 410 core


out vec4 outColour;
uniform mat4 MVP;
uniform mat4 view;
uniform mat4 projection;
uniform samplerBuffer posSampler; // index 0
uniform samplerBuffer colourSampler; // index 1
uniform samplerBuffer scaleSampler; // index 2


void main()
{
    vec3 inPos=texelFetch(posSampler,gl_VertexID).xyz;
    vec4 inColour=texelFetch(colourSampler,gl_VertexID);
    vec3 scale=texelFetch(scaleSampler,gl_VertexID).xyz;
    //vec3 scaled=scale*inPos;
    gl_Position = projection * view * vec4(inPos, 1.0);
    outColour = inColour;
}
