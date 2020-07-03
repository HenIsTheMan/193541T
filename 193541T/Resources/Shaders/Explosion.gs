#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in myInterface{ //Output interface block
    vec2 TexCoords;
} gsIn[];
out vec2 TexCoords;

uniform float magnitude;
uniform float time;

vec4 Explode(vec4 pos, vec3 normal){
    vec3 direction = normal * ((sin(time) + 1.f) / 2.f) * magnitude; 
    return pos + vec4(direction, 0.f);
}

vec3 GetNormal(){
    return normalize(cross(vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position), vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position)));
}

void main(){
    vec3 normal = GetNormal();

    gl_Position = Explode(gl_in[0].gl_Position, normal);
    TexCoords = gsIn[0].TexCoords;
    EmitVertex();

    gl_Position = Explode(gl_in[1].gl_Position, normal);
    TexCoords = gsIn[1].TexCoords;
    EmitVertex();

    gl_Position = Explode(gl_in[2].gl_Position, normal);
    TexCoords = gsIn[2].TexCoords;
    EmitVertex();

    EndPrimitive();
}