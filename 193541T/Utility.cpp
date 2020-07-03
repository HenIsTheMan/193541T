#include "Utility.h"

Vertex::Vertex(glm::vec3 pos, glm::vec4 colour, glm::vec2 texCoords, glm::vec3 normal){
    this->pos = pos;
    this->colour = colour;
    this->texCoords = texCoords;
    this->normal = normal;
}

Texture::Texture(uint newRefID, str newType): refID(newRefID), type(newType){}