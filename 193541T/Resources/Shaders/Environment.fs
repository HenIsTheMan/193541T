#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 camPos;
uniform samplerCube skybox;
uniform sampler2D map[32];
uniform int mapCount;

void main(){
    const float ratio = 1.f / 1.52f; //n of air / n of glass (ratio between refractive indices of both materials)
    vec3 I = normalize(FragPos - camPos);
    vec3 R = reflect(I, normalize(Normal)); //Reflection vec
    //vec3 R = refract(I, normalize(Normal), ratio); //Refraction vec //Makes cube act as a magnifying glass

    ///Use either of the vecs to index/sample cubemap
    vec3 col = texture(skybox, R).rgb;
    for(int i = 0; i < mapCount; ++i){
        col += vec3(texture(map[i], TexCoords));
    }
    FragColor = vec4(col, 1.f);
}