#pragma once
#include "Src.h"
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

class Mesh{ //Single drawable entity
    uint VAO, VBO, EBO; //Store ref ID
    void SetupMesh();
public:
    std::vector<Vertex> vertices;
    std::vector<uint> indices;
    std::vector<Texture> textures;
    Mesh(std::vector<Vertex>, std::vector<uint>);
    void LoadTexture(cstr, str);
    void Draw(bool, bool) const;
    void DrawPts(uint) const;
};