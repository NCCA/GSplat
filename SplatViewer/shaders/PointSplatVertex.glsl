#version 410 core

layout (location=0) in vec3 inPosition;
layout (location=1) in vec4 colour;
layout (location=2) in mat3 rotations;

out vec4 outColour;
uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(inPosition, 1.0);
    outColour = colour;
}
