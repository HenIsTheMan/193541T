#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

/*layout (std140) uniform Matrices{ //std140 is the uni block layout (mem layout used by a uni block to store its content)
    mat4 projection;
    mat4 view;
};
uniform mat4 model;*/
//layout(std140, binding = 2) uniform example{}; //Another layout qualifier/specifier to set binding pt explicitly

void main(){
    gl_Position = projection * view * model * vec4(aPos, 1.f);    
    gl_PointSize = gl_Position.z; //Vertex dist to viewer
}  