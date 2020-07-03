#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

out vec3 ourColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * view * model * vec4(aPos, 1.f);
    ourColor = aColor;
    TexCoord = aTexCoord;
    Normal = mat3(transpose(inverse(model))) * aNormal; //Multiplication with normal matrix //Cast to mat3
    FragPos = vec3(model * vec4(aPos, 1.f)); //Vertex pos in world space //Cast to vec3
}