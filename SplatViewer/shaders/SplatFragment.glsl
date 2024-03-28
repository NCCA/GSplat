#version 410 core

layout (location = 0) out vec4 fragColour;

in vec4 colour;
in vec3 pointCenter;
in vec3 actualPoint;
uniform vec3 eye;

vec3 sphereIntersect(vec3 c, vec3 ro, vec3 p) {
    vec3 rd = vec3(normalize(p - ro));
    vec3 u = vec3(ro - c); // ro relative to c

    float a = dot(rd, rd);
    float b = 2.0 * dot(u, rd);
    float cc = dot(u, u) - 1.0;

    float discriminant = b * b - 4 * a * cc;

    // no intersection
    if (discriminant < 0.0) {
        return vec3(0.0);
    }

    float t1 = (-b + sqrt(discriminant)) / (2.0 * a);
    float t2 = (-b - sqrt(discriminant)) / (2.0 * a);
    float t = min(t1, t2);
    vec3 intersection = ro + vec3(t * rd);

    return intersection;
}

void main()
{



    fragColour = colour;
}



//#version 410 core
//
//in vec4 vColor;
//in vec2 vPosition;
//
//out vec4 fragColor;
//
//void main ()
//{
//    float A = -dot(vPosition, vPosition);
//    if (A < -4.0) discard;
//    float B = exp(A) * vColor.a;
//    fragColor = vec4(B * vColor.rgb, B);
    //}
