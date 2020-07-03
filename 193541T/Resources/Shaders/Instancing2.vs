/*#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 5) in mat4 instanceMatrix;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main(){
    gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.f); 
    TexCoords = aTexCoords;
}*/

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aNormal;
layout (location = 5) in mat4 instanceMatrix;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.f); 
    TexCoords = aTexCoords;
    Normal = mat3(transpose(inverse(model))) * aNormal; //Multiplication with normal matrix
    FragPos = vec3(model * vec4(aPos, 1.f)); //Vertex pos in world space //Cast to vec3
}