#version 410 core
layout(points) in;
//layout (points, max_vertices = 1) out;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

in vec4 pointColour[];
in mat3 rotation[];
out vec4 colour;
out vec3 pointCenter;
out vec3 actualPoint;
out vec2 uv;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    colour = pointColour[0];

    vec3 pos = gl_in[0].gl_Position.xyz;
    // now generate the four vertices of the quad
    vec3 v1=rotation[0]*vec3(-1,-1,0);
    vec3 v2=rotation[0]*vec3(1,-1,0);
    vec3 v3=rotation[0]*vec3(-1,1,0);
    vec3 v4=rotation[0]*vec3(1,1,0);
    vec2 uv1=vec2(0,0);
    vec2 uv2=vec2(1,0);
    vec2 uv3=vec2(0,1);
    vec2 uv4=vec2(1,1);

    gl_Position = projection * view * vec4(pos +v1, 1.0);
    pointCenter=pos;
    actualPoint=v1;
    uv=uv1;
    EmitVertex();
    gl_Position = projection * view * vec4(pos + v2, 1.0);
    pointCenter=pos;
    actualPoint=v2;
    uv=uv2;
    EmitVertex();
    gl_Position = projection * view * vec4(pos + v3, 1.0);
    pointCenter=pos;
    actualPoint=v3;
    uv=uv3;
    EmitVertex();
    gl_Position = projection * view * vec4(pos + v4, 1.0);
    pointCenter=pos;
    actualPoint=v4;
    uv=uv4;
    EmitVertex();
    EndPrimitive();



//    gl_Position = gl_in[0].gl_Position;
//    EmitVertex();
//    EndPrimitive();
}