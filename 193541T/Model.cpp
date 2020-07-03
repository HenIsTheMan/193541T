#include "Model.h"

Model::Model(cstr fPath){
    directoryHead = str(fPath).substr(0, str(fPath).find_first_of('/') + 1) + "Textures/";
    LoadModel(fPath);
}

void Model::LoadModel(cstr fPath){ //Load model into a DS of Assimp called a scene obj (root obj of Assimp's data interface)
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(fPath, aiProcess_Triangulate | aiProcess_FlipUVs);
    if(!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE){ //If !scene || !(root node of scene) || returned data is incomplete (given by 1 of its flags)
        printf("Assimp error: %s\n", import.GetErrorString());
        return;
    }
    ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene){ //Process all of the scene obj's nodes recursively to translate the loaded data to an arr of Mesh objs //For parent-child relation between meshes
    for(uint i = 0; i < node->mNumMeshes; ++i){ //Process all the nodes' meshes (if any)
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; //Check a node's mesh indices and retrieve the corresponding mesh by indexing the scene's mMeshes array
        meshes.emplace_back(ProcessMesh(mesh, scene)); //Store mesh obj
    }
    for(uint i = 0; i < node->mNumChildren /*base case/exit condition/terminating condition*/; ++i){
        ProcessNode(node->mChildren[i], scene); //Each node contains a set of mesh indices where each points to a specific mesh located in the scene obj
    }
}

Mesh Model::ProcessMesh(aiMesh* meshObj, const aiScene* scene){
    std::vector<Vertex> vertices;
    std::vector<uint> indices;

    for(uint i = 0; i < meshObj->mNumVertices; ++i){ //For each vertex of the mesh...
        vertices.emplace_back(Vertex(
            glm::vec3(meshObj->mVertices[i].x, meshObj->mVertices[i].y, meshObj->mVertices[i].z),
            (meshObj->mColors[0] ? glm::vec4(meshObj->mColors[0][i].r, meshObj->mColors[0][i].g, meshObj->mColors[0][i].b, meshObj->mColors[0][i].a) : glm::vec4(0.f)),
            (meshObj->mTextureCoords[0] ? glm::vec2(meshObj->mTextureCoords[0][i].x, meshObj->mTextureCoords[0][i].y) : glm::vec2(0.f)), //Assimp allows a model to have 8 diff texCoords per vertex //Check if mesh has texCoords before...
            glm::vec3(meshObj->mNormals[i].x, meshObj->mNormals[i].y, meshObj->mNormals[i].z)));
    }
    for(uint i = 0; i < meshObj->mNumFaces; ++i){ //For each face of the mesh... //Each mesh has an arr of primitive faces (triangles due to the aiProcess_Triangulate post-processing option)
        aiFace face = meshObj->mFaces[i]; //Contains indices defining which vertices to draw and in what order for each primitive //Placeholder
        for(uint j = 0; j < face.mNumIndices; ++j){
            indices.emplace_back(face.mIndices[j]);
        }
    }
    Mesh mesh(vertices, indices);
    if(meshObj->mMaterialIndex >= 0){ //Query the mesh's material index to check if the mesh contains a material
        aiMaterial* matObj = scene->mMaterials[meshObj->mMaterialIndex]; //Just like with nodes, a mesh only contains an index to a material obj so retrieve aiMaterial obj (stores the material data, contains an arr of texture locations for each texture type) by indexing...
        std::vector<std::pair<str, str>> texMaps;
        LoadMaterialTextures(matObj, texMaps);
        for(const auto& texMap: texMaps){
            mesh.LoadTexture(texMap.first.c_str(), texMap.second);
        }
    }
    return mesh;
}

void Model::LoadMaterialTextures(const aiMaterial* const mat, std::vector<std::pair<str, str>>& texMaps){ //Helper function to retrieve the textures from the material
    aiTextureType types[]{aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_NORMAL_CAMERA, aiTextureType_AMBIENT};
    for(short i = 0; i < sizeof(types) / sizeof(types[0]); ++i){
        for(uint j = 0; j < mat->GetTextureCount(types[i]); ++j){ //For each texture in the material of the given texture type...
            aiString aiStr;
            mat->GetTexture(types[i], j, &aiStr); //Retrieve each texture's file location
            bool texAlrLoaded = 0;
            str newFileName = directoryHead + str(aiStr.C_Str()), typeName;
            switch(i){
                case 0: typeName = "d"; break;
                case 1: typeName = "s"; break;
                case 2: typeName = "n"; break;
                case 3: typeName = "r";
            }
            if(!texAlrLoaded){
                texMaps.emplace_back(std::make_pair(newFileName, typeName));
            }
        }
    }
}

void Model::Draw(bool indexed, bool tex) const{
    for(auto mesh: meshes){
        mesh.Draw(indexed, tex);
    }
}

void Model::Draw2() const{
    for(auto mesh : meshes){
        mesh.Draw2();
    }
}