#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in myInterface{
    vec4 Colour;
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
} gsIn[];

out myInterface{
    vec4 Colour;
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
} gsOut;

uniform float magnitude;
uniform float time;
uniform mat4 MVP;

vec4 Explode(vec4 pos, vec3 normal){
    return pos + vec4(normal * ((sin(time) + 1.f) / 2.f) * magnitude, 0.f);
}

vec3 CalcNormal(){
    return cross(vec3(gl_in[2].gl_Position - gl_in[1].gl_Position), (gl_in[0].gl_Position - gl_in[1].gl_Position).xyz);
}

void main(){
    vec3 normal = normalize(CalcNormal());
    for(int i = 0; i < 3; ++i){
        gl_Position = MVP * Explode(gl_in[i].gl_Position, normal);
        gsOut.Colour = gsIn[i].Colour;
        gsOut.TexCoords = gsIn[i].TexCoords;
        gsOut.Normal = gsIn[i].Normal;
        gsOut.FragPos = gsIn[i].FragPos;
        EmitVertex();
    }
    EndPrimitive();
}