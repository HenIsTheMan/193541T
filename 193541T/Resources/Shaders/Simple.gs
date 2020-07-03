///Pass-through geometry shader (takes a primitive as its input and passes it to the next shader unmodified)
#version 330 core
layout (points) in;
layout (points, max_vertices = 1) out;

in vec3 myColour[];
out vec3 ourColour;

void main(){
    gl_Position = gl_in[0].gl_Position;
    ourColour = myColour[0];
    EmitVertex();
    EndPrimitive();
}