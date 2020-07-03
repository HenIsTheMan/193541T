#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in myInterface{
    vec4 Colour;
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
    vec3 TexDir;
} gsIn[];

out myInterface{
    vec4 Colour;
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
    vec3 TexDir;
} gsOut;

uniform float magnitude;
uniform float time;

vec4 Explode(vec4 pos, vec3 normal){
    vec3 dir = normal * ((sin(time) + 1.f) / 2.f) * magnitude; //Range of ((sin(time) + 1.f) / 2.f): [0, 1]
    return pos + vec4(dir, 0.f);
}

vec3 GetNormal(){
    return normalize(cross(vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position), vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position))); //Order matters
}

void main(){
    vec3 normal = GetNormal();
    for(int i = 0; i < 3; ++i){
        gl_Position = Explode(gl_in[i].gl_Position, normal);
        gsOut.TexCoords = gsIn[i].TexCoords;
        EmitVertex();
    }
    EndPrimitive();
}