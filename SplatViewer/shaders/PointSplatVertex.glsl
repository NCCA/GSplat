#version 410 core


out vec4 outColour;
uniform mat4 MVP;
uniform mat4 view;
uniform mat4 projection;
uniform samplerBuffer posSampler;
uniform samplerBuffer colourSampler;

void main()
{
    vec3 inPos=texelFetch(posSampler,gl_VertexID).xyz;
    vec4 inColour=texelFetch(colourSampler,gl_VertexID);
    gl_Position = projection * view * vec4(inPos, 1.0);
    outColour = inColour;
}
