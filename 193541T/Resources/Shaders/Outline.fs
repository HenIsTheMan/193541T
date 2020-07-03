#version 330 core
out vec4 FragColor;

uniform vec3 myRGB;
//uniform float myAlpha;

void main(){
    FragColor = vec4(myRGB, 1.f);
}