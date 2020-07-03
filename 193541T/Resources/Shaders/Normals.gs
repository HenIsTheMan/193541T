#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT{
    vec3 normal;
} gsIn[];

const float length = .7f;

void GenLine(int index){
    for(float i = 0.f; i < 2.f; ++i){
        gl_Position = gl_in[index].gl_Position + i * length * vec4(gsIn[index].normal, 0.f); //Normalise instead??
        EmitVertex();
    }
    EndPrimitive();
}

void main(){
    for(int i = 0; i < 3; ++i){ 
        GenLine(i);
    }
}