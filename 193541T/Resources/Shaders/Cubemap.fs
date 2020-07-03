#version 330 core
out vec4 FragColor;

in vec3 texDir; //Dir vec representing a 3D texCoord for indexing/sampling the cubemap (similar to the pos vec of an interpolated local vertex pos of a cube) //Can be a non-unit vec

uniform samplerCube cubemap;

void main(){
    FragColor = texture(cubemap, texDir);
}