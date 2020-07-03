#version 330 core
out vec4 FragColor;

struct Material{
    //Ambient component (sets colour the surface reflects under ambient lighting) removed as ambience of fragment = light.ambient * material.diffuse;
    ///sampler2D is an opaque type (cannot be instantiated but can only be defined as uniforms)
    sampler2D dMap[20]; //Stores diffuse map (texture) //Sets colour the surface reflects under diffuse lighting
    sampler2D sMap[20]; //Stores specular map (texture) //Sets colour of specular highlight on surface or possibly reflect a surface-specific color //Coloured specular map results in coloured specular highlights
    sampler2D eMap[20]; //Stores emission map (texture, stores emission values [colours an obj might emit as if it contains a light source itself] per fragment)
    sampler2D nMap[20];
    sampler2D rMap[20]; //Stores reflection map (texture, determine which parts of the model reflect light and by what intensity)
    float shininess; //Impacts the scattering of light and hence radius of the specular highlight
    //++mu
    //++volumetric mass density
    //PBR??
};
uniform Material material;

struct PointLight{ //Positional light source
    vec3 pos;
    float constant; //Constant term //Makes sure the denominator >= 1.f
    float linear; //Linear term //Multiplied with dist to reduce light intensity in a linear fashion
    float quadratic; //Quadratic term //Multiplied with the quadrant of the dist to set quadratic decreases in light intensity //Less significant compared to linear term when dist is small
};  
#define POINT_AMT 4
uniform PointLight pLights[POINT_AMT];

struct DirectionalLight{ //Directional light source
    vec3 dir;
};  
uniform DirectionalLight dLight;

struct Spotlight{ //Positional light that shoots light rays in 1 dir, objs within its cut-offs (a certain radius of its dir) are lit
    vec3 pos;
    vec3 dir;
    float cosInnerCutoff; //cos(inner cut-off angle [phi])
    float cosOuterCutoff; //cos(outer cut-off angle [phi])
};
uniform Spotlight sLight;

in vec3 ourColor; //??
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
uniform vec3 viewPos;

///Light properties (++ w-component??) 
vec3 lightAmbient = vec3(.05f); //Ambient component of light //Small value so it has a small impact
vec3 lightDiffuse = vec3(.8f); //Diffuse component of light //Set to desired colour of light
vec3 lightSpecular = vec3(1.f); //Specular component of light //Usually kept at vec3(1.f)

vec3 normal = normalize(Normal);
uniform int dMapCount;
uniform int sMapCount;
uniform int eMapCount;
uniform int nMapCount;
uniform int rMapCount;

vec3 CalcAmbient(){ //Ambient lighting (ensures objs always have colour due to light from distant light source(s))
    vec3 ambient = vec3(lightAmbient);
    for(int i = 0; i < dMapCount; ++i){
        ambient *= vec3(texture(material.dMap[i], TexCoords));
    }
    return ambient;
}

vec3 CalcDiffuse(vec3 lightDir){ //Diffuse lighting (simulates directional impact of light, light intensity is affected by facing of obj)
    float diff = max(dot(normal, -lightDir), 0.0); //Diffuse impact of light on curr fragment
    vec3 diffuse = vec3(diff);
    for(int i = 0; i < dMapCount; ++i){
        diffuse *= vec3(texture(material.dMap[i], TexCoords));
    }
    return lightDiffuse * diffuse; //Diffuse component (> 0.f && <= 1.f when angle between... (>= 0.f && < 90.f) || (> 270.f && <= 360.f))
}

vec3 CalcSpecular(vec3 lightDir){ //Specular lighting (simulates bright spot of light that appears on shiny objs, specular highlights are more inclined to light colour than obj colour)
    vec3 viewDir = normalize(viewPos - FragPos); //Dir of view vec //-ve??
    vec3 reflectDir = reflect(lightDir, normal); //Dir vec of reflected light ray //Not -lightDir as reflect(...) expects the 1st vec to point from the light source to the curr fragment's pos
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = vec3(spec);
    for(int i = 0; i < sMapCount; ++i){
        specular *= vec3(texture(material.sMap[i], TexCoords));
    }
    return lightSpecular * specular; //Can invert colours
}

vec3 CalcPointLight(PointLight light){ //Calc point light's contribution vec
    vec3 lightDir = normalize(FragPos - light.pos); //Dir of directed light ray (diff vec between...)

    ///Attenuation (reducing the intensity of light over the dist it travels)
    float dist = length(FragPos - light.pos);
    float attenuation = 1.f / (light.constant + light.linear * dist + light.quadratic * dist * dist); //Better than simply a linear eqn
    return attenuation * (CalcAmbient() + CalcDiffuse(lightDir) + CalcSpecular(lightDir));
}

vec3 CalcDirectionalLight(DirectionalLight light){ //Calc directional light's contribution vec
    vec3 lightDir = normalize(light.dir);
    return CalcAmbient() + CalcDiffuse(lightDir) + CalcSpecular(lightDir);
}

vec3 CalcSpotlight(Spotlight light){ //Calc spotlight's contribution vec
    vec3 lightDir = normalize(FragPos - light.pos); //Dir of directed light ray (diff vec between...)

    ///Soft/Smooth edges (using inner and outer cone, interpolate between outer cos and inner cos based on theta)
    float cosTheta = dot(lightDir, normalize(light.dir));
    float epsilon = light.cosInnerCutoff - light.cosOuterCutoff;
    float lightIntensity = clamp((cosTheta - light.cosOuterCutoff) / epsilon, 0.f, 1.f); //-ve when outside the outer cone of the spotlight and > 1.f when inside... before clamping
    return CalcAmbient() + CalcDiffuse(lightDir) * lightIntensity + CalcSpecular(lightDir) * lightIntensity; //Leave ambient component unaffected so length(ambient) > 0
}

void main(){ //With Phong lighting model
    vec3 emission = vec3(1.f), normal = vec3(1.f), reflection = vec3(1.f);
    for(int i = 0; i < eMapCount; ++i){
        emission *= vec3(texture(material.eMap[i], TexCoords));
    }
    for(int i = 0; i < nMapCount; ++i){
        normal *= vec3(texture(material.nMap[i], TexCoords));
    }
    for(int i = 0; i < rMapCount; ++i){
        reflection *= vec3(texture(material.rMap[i], TexCoords));
    }
    vec3 result = CalcDirectionalLight(dLight) + CalcSpotlight(sLight) + (eMapCount != 0 ? emission : vec3(0.f)) + (nMapCount != 0 ? normal : vec3(0.f)) + (rMapCount != 0 ? reflection : vec3(0.f));
    for(int i = 0; i < POINT_AMT; ++i){
        result += CalcPointLight(pLights[i]);
    }
    FragColor = vec4(result, 1.f); //texture(...) returns the colour of the texture at an interpolated set of texture coordinates
}