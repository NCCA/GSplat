#version 410 core

in vec4 vColor;
in vec2 vPosition;

out vec4 fragColor;

void main ()
{
    float A = -dot(vPosition, vPosition);
    if (A < -4.0) discard;
    float B = exp(A) * vColor.a;
    fragColor = vec4(B * vColor.rgb, B);
}
