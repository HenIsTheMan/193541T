#pragma once
#include "Cam.h"
#include "Mesh.h"
#include "Model.h"
#include "ShaderChief.h"

class Scene final{
    glm::vec3 quadPos[5]{
        glm::vec3(1.5f, 1.0f, -10.5f),
        glm::vec3(1.5f, 1.5f, 2.5f),
        glm::vec3(0.0f, 12.f, -5.f),
        glm::vec3(-0.5f, 1.0f, -6.f),
        glm::vec3(0.5f, -1.5f, -1.5f)
    };
    std::vector<cstr> texFaces{
        "Resources/Textures/Skybox/Right.jpg",
        "Resources/Textures/Skybox/Left.jpg",
        "Resources/Textures/Skybox/Top.jpg",
        "Resources/Textures/Skybox/Bottom.jpg",
        "Resources/Textures/Skybox/Front.jpg",
        "Resources/Textures/Skybox/Back.jpg"
    };
	Mesh* meshes[4];
    Model* models[4];
	ShaderChief* basicSC;
	ShaderChief* outlineSC;
	ShaderChief* quadSC;
	ShaderChief* screenQuadSC;
	ShaderChief* cubemapSC;
	ShaderChief* ptSC;
	ShaderChief* normalsSC;
	ShaderChief* instancingSC;
	ShaderChief* instancing2SC;
    uint cubemapRefID;
    const std::vector<Vertex> LoadQuadVertices() const;
    const std::vector<Vertex> LoadCubeVertices() const;
    const std::vector<Vertex> LoadPtVertices() const;
    const std::vector<Vertex> LoadQuadVertices2() const;
    const uint CreateCubemap(const std::vector<cstr>&) const;
    void DrawInstance(const Cam&, const glm::vec3&, const glm::vec3&) const;
    void RenderShiny(const Cam&, const glm::vec3&, const glm::vec3&, bool) const;
    void RenderSkybox(const Cam&) const;
    void RenderWindows(const Cam&) const;
    void SetUnis(const Cam&, short = 0, const glm::vec3& = glm::vec3(0.f), const glm::vec3& = glm::vec3(1.f), const glm::vec4& = {0.f, 1.f, 0.f, 0.f}) const;
public:
	Scene();
	~Scene();
    void RenderToCreatedFB(const Cam const& const, const uint* const& const) const;
    void RenderToDefaultFB(const uint&, const glm::vec3 & = glm::vec3(0.f), const glm::vec3 & = glm::vec3(1.f)) const;
};