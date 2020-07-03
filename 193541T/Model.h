#pragma once
#include "Src.h"
#include "Mesh.h"

class Model{ //A model obj acts as a container for several mesh objs
    std::vector<std::pair<str, str>> texLoaded;
    std::vector<Mesh> meshes;
    str directoryHead;
    void LoadModel(cstr);
    void ProcessNode(aiNode*, const aiScene*);
    Mesh ProcessMesh(aiMesh*, const aiScene*);
    void LoadMaterialTextures(const aiMaterial* const&, std::vector<std::pair<str, str>>&);
public:
    Model(cstr);
    void Draw(bool = 1, bool = 1) const;
};