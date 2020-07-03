#pragma once
#include "ShaderChief.h"

struct Vertex{ //Can use to index each vertex attrib
    Vertex(glm::vec3, glm::vec4, glm::vec2, glm::vec3);
    glm::vec3 pos;
    glm::vec4 colour;
    glm::vec2 texCoords;
    glm::vec3 normal;
};

struct Texture{
    Texture(uint, str);
    uint refID;
    str type;
};