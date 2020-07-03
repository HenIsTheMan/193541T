#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 texDir;

uniform mat4 projection;
uniform mat4 view;

void main(){
    gl_Position = projection * view * vec4(aPos, 1.f);
    gl_Position = gl_Position.xyww; //Resulting NDC after perspective division will have a z value (gl_FragCoord.z) equal to 1.f (max depth value) so skybox appears behind all other objs
    texDir = aPos;
}