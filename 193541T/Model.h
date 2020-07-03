#pragma once
#include "Src.h"
#include "Mesh.h"

class Model final{ //A model obj acts as a container for several mesh objs
    cstr modelPath;
    std::vector<std::pair<str, str>> texLoaded;
    mutable std::vector<Mesh> meshes;
    str directoryHead;
    void Init();
    void LoadModel(cstr) const;
    void ProcessNode(aiNode*, const aiScene*) const;
    Mesh ProcessMesh(aiMesh*, const aiScene*) const;
    void LoadMaterialTextures(const aiMaterial* const&, const std::vector<std::pair<str, str>>&) const;
public:
    Model(cstr);
    void Draw(bool = 1, bool = 1);
    void DrawInstanced(bool, bool, uint = 0);
};