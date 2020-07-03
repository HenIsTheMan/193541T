#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 4) in vec2 aOffset;

//uniform vec2 offsets[100];
uniform mat4 MVP;

void main(){
    //gl_Position = MVP * vec4(aPos + offsets[gl_InstanceID], 0.f, 1.f); //gl_InstanceID is incremented for each instanced draw/... call
    vec2 pos = aPos * (gl_InstanceID / 100.f); //Downscaled pos
    gl_Position = MVP * vec4(pos + aOffset, 0.f, 1.f);
}