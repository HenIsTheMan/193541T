#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main(){
    /*vec4 texColor = texture(texture1, TexCoords); //Use FragColor??
    if(texColor.a < 0.1){
        discard; //Fragment is discarded and not stored in the color buffer
    }*/
    FragColor = texture(texture1, TexCoords);
}