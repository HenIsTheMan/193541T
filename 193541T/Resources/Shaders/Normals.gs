#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in myInterface{
    vec4 Colour;
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
    vec3 TexDir;
} gsIn[];

const float len = .7f;
void GenLine(int index){
    for(float i = 0.f; i < 2.f; ++i){
        gl_Position = gl_in[index].gl_Position + i * len * vec4(gsIn[index].Normal, 0.f); //Normalise instead??
        EmitVertex();
    }
    EndPrimitive();
}

void main(){
    for(int i = 0; i < 3; ++i){ 
        GenLine(i);
    }
}