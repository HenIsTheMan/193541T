#pragma once
#include "Src.h"
#include "Utility.h"

class Mesh final{ //Single drawable entity
    glm::vec2 translations[100];
    uint VAO, VBO, EBO; //Store ref ID
    void Init(uint = 1);
public:
    std::vector<Vertex> vertices;
    std::vector<uint> indices;
    std::vector<Texture> textures;
    Mesh(std::vector<Vertex>, std::vector<uint>);
    void LoadTexture(cstr, str);
    void Draw(bool, bool);
    void DrawPts(uint = 1);
    void DrawInstanced(bool, bool, uint);
};