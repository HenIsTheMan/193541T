#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in myInterface{
    vec4 Colour;
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
} gsIn[];

uniform float len;

void GenLine(int index){
    for(float i = 0.f; i <= len; i += len){
        gl_Position = gl_in[index].gl_Position + i * vec4(normalize(gsIn[index].Normal), 0.f);
        EmitVertex();
    }
    EndPrimitive();
}

void main(){
    for(int i = 0; i < 3; ++i){ 
        GenLine(i);
    }
}