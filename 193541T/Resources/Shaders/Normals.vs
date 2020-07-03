#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec3 aNormal; //Vertex normals supplied by the model (how??)

out VS_OUT{
    vec3 normal;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 MVP;

void main(){
    gl_Position = MVP * vec4(aPos, 1.f); //Clip-space pos vec
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = normalize(vec3(projection * vec4(normalMatrix * aNormal, 0.f))); //Transformed clip-space normal vec //Multiply by normalMatrix to acct for scaling and rotations due to view and model matrix??
}