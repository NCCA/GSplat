#version 410 core

layout (location = 0) out vec4 fragColour;

in vec4 outColour;

void main()
{
    fragColour = outColour;
}

