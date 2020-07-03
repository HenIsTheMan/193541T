#version 330 core
out vec4 FragColor;

struct Material{ //Define a material color for each of the 3 lighting components
    vec3 ambient; //Color the surface reflects under ambient lighting
    vec3 diffuse; //...
    vec3 specular; //...
    float shininess; //Impacts the scattering/radius of the specular highlight
    //++mu
    //++density
};
uniform Material material;

struct Light{
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

in vec3 ourColor; //??
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform float proportion;
///Arr??
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 viewPos;

void main(){ //With Phong lighting model
    ///Ambient lighting (ensures objs always have colour due to light from distant light source(s))
    /*float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor; //Ambient component

    vec3 norm = normalize(Normal); //normalize??
    vec3 lightDir = -normalize(lightPos - FragPos); //Dir of directed light ray (diff vec between...)

    ///Diffuse lighting (simulates directional impact of light, light intensity is affected by facing of obj)
    float diff = max(dot(norm, -lightDir), 0.f); //Diffuse impact of light on curr fragment
    vec3 diffuse = diff * lightColor; //Diffuse component (> 0.f && <= 1.f when angle between... (>= 0.f && < 90.f) || (> 270.f && <= 360.f))

    ///Specular lighting (simulates bright spot of light that appears on shiny objs, specular highlights are more inclined to light colour than obj colour)
    float specularStrength = 0.8; //Specular intensity
    vec3 viewDir = normalize(viewPos - FragPos); //Dir of view vec
    vec3 reflectDir = reflect(lightDir, norm); //Dir vec of reflected light ray //Not -lightDir as reflect(...) expects the 1st vec to point from the light source to the curr fragment's pos
    int shininess = 256; //Determines focus and hence size of specular highlight //Make into uni??
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;  

    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), proportion) * vec4((ambient + diffuse + specular) * objectColor, 1.f);*/ //texture(...) returns the filtered color of the texture at an interpolated set of texture coordinates

    //Ambient
    vec3 ambient = light.ambient * material.ambient;
  	
    //Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.pos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = light.diffuse * (diff * material.diffuse);

    //Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    FragColor = vec4(ambient + diffuse + specular, 1.f);
}