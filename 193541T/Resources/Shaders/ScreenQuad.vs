#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out myInterface{ //Output interface block
    vec2 TexCoords;
} vsOut; //Instance

uniform mat4 model;

void main(){
    gl_Position = model * vec4(aPos.x, aPos.y, 0.f, 1.f); 
    vsOut.TexCoords = aTexCoords;
}

//REMOVE