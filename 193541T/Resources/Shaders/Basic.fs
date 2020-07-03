#version 330 core
out vec4 FragColor;

struct Mat{
    sampler2D dMap; //Sets colour the surface reflects under diffuse lighting
    sampler2D sMap; //Sets colour of specular highlight on surface or possibly reflect a surface-specific color //Coloured specular map results in coloured specular highlights
    sampler2D eMap; //Stores tex unit of emission map (stores emission values [colours an obj might emit as if it contains a light source itself] per fragment)
    sampler2D nMap; //sampler2D is an opaque type (cannot be instantiated but can only be defined as uniforms)
    sampler2D rMap; //Stores reflection map (texture, determine which parts of the model reflect light and by what intensity)
    float shininess; //Impacts the scattering of light and hence radius of the specular highlight
    //++mu //++volumetric mass density //PBR??
};

struct PointLight{ //Positional light source
    vec3 pos;
    float constant; //Constant term //Makes sure the denominator >= 1.f
    float linear; //Linear term //Multiplied with dist to reduce light intensity in a linear fashion
    float quadratic; //Quadratic term //Multiplied with the quadrant of the dist to set quadratic decreases in light intensity //Less significant compared to linear term when dist is small
};

struct DirectionalLight{ //Directional light source
    vec3 dir;
};

struct Spotlight{ //Positional light that shoots light rays in 1 dir, objs within its cut-offs (a certain radius of its dir) are lit
    vec3 pos;
    vec3 dir;
    float cosInnerCutoff; //cos(inner cut-off angle [phi])
    float cosOuterCutoff; //cos(outer cut-off angle [phi])
};

#define ptLightAmt 4
uniform Mat material;
uniform PointLight pLights[ptLightAmt];
uniform DirectionalLight dLight;
uniform Spotlight sLight;

in vec3 ourColour;
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
vec3 normal = normalize(Normal);
uniform vec3 camPos;
uniform samplerCube skybox;
uniform bool useFlatColour;

///Light properties (++ w-component??) 
const vec3 lightAmbient = vec3(.4f); //Ambient component of light //Small value so it has a small impact
const vec3 lightDiffuse = vec3(.8f); //Diffuse component of light //Set to desired colour of light
const vec3 lightSpecular = vec3(1.f); //Specular component of light //Usually kept at vec3(1.f)

vec3 CalcAmbient(){ //Ambient lighting (ensures objs always have colour due to light from distant light source(s))
    return lightAmbient * texture(material.dMap, TexCoords).rgb; //Ambient component of light * that of fragment's material
}

vec3 CalcDiffuse(vec3 lightDir){ //Diffuse lighting (simulates directional impact of light, light intensity is affected by facing of obj)
    float dImpact = max(dot(normal, -lightDir), 0.f); //Diffuse impact of light on curr fragment
    return dImpact * lightDiffuse * texture(material.dMap, TexCoords).rgb; //Diffuse component (> 0.f && <= 1.f when angle between... (>= 0.f && < 90.f) || (> 270.f && <= 360.f)) of fragment
}

vec3 CalcSpecular(vec3 lightDir){ //Specular lighting (simulates bright spot of light that appears on shiny objs, specular highlights are more inclined to light colour than obj colour)
    vec3 viewDir = normalize(FragPos - camPos);
    vec3 reflectDir = reflect(lightDir, normal); //Not -lightDir as reflect(...) expects the 1st vec to point from the light source to the curr fragment's pos
    float sImpact = pow(max(dot(viewDir, reflectDir), 0.f), material.shininess);
    return sImpact * lightSpecular * texture(material.sMap, TexCoords).rgb;
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

uniform bool emission, bump, reflection;

void main(){ //With Phong lighting model
    const float ratio = 1.f / 1.52f; //n of air / n of glass (ratio between refractive indices of both materials)
    vec3 incidentRay = normalize(FragPos - camPos);
    vec3 reflectedRay = reflect(incidentRay, normal);
    vec3 refractedRay1st = refract(incidentRay, normal, ratio);
    vec3 refractedRay2nd = refract(refractedRay1st, normal, 1.f / ratio); //Wrong normal??

    vec3 result = CalcDirectionalLight(dLight) + CalcSpotlight(sLight);
    for(int i = 0; i < ptLightAmt; ++i){
        result += CalcPointLight(pLights[i]);
    }
    if(reflection){ //For environment mapping
        result *= mix(texture(skybox, reflectedRay).rgb, texture(skybox, refractedRay2nd).rgb, 0.5) * vec3(texture(material.rMap, TexCoords));
    }
    if(bump){
        result *= vec3(texture(material.nMap, TexCoords));
    }
    if(emission){
        result *= vec3(texture(material.eMap, TexCoords));
    }
    FragColor = vec4(useFlatColour ? ourColour : result, 1.f); //texture(...) returns colour of tex at an interpolated set of texCoords
}

//Lighting, reflection, refraction and vertex movement for water rendering??