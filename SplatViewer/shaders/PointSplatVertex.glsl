#version 410 core

layout (location=0) in vec3 inPosition;
layout (location=1) in vec4 colour;
layout (location=2) in mat3 rotations;

out vec4 outColour;
uniform mat4 MVP;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    gl_Position = projection * view * vec4(inPosition, 1.0);
    //outColour = colour;
    outColour = vec4(colour.rgb * 0.28 + vec3(0.5, 0.5, 0.5),colour.a);

}
