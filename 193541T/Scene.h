#pragma once
#include "Cam.h"
#include "Mesh.h"
#include "Model.h"
#include "ShaderChief.h"

class Scene final{
    glm::vec3 quadPos[5]{
        glm::vec3(-1.5f, 0.0f, -0.5f),
        glm::vec3(1.5f, 0.0f, 2.5f),
        glm::vec3(0.0f, 0.0f, 3.9f),
        glm::vec3(-0.3f, 0.0f, -5.3f),
        glm::vec3(0.5f, 0.0f, -4.2f)
    };
    std::vector<cstr> texFaces{
        "Resources/Textures/Skybox/Right.jpg",
        "Resources/Textures/Skybox/Left.jpg",
        "Resources/Textures/Skybox/Top.jpg",
        "Resources/Textures/Skybox/Bottom.jpg",
        "Resources/Textures/Skybox/Front.jpg",
        "Resources/Textures/Skybox/Back.jpg"
    };
	Mesh* meshes[2];
    Model* models[2];
	ShaderChief* basicSC;
	ShaderChief* outlineSC;
	ShaderChief* quadSC;
	ShaderChief* screenQuadSC;
	ShaderChief* cubemapSC;
    uint cubemapRefID;
	const std::vector<Vertex> LoadQuadVertices() const, LoadCubeVertices() const;
    const uint LoadCubemap(const std::vector<cstr>&) const;
    void DrawInstance(const Cam&, const glm::vec3&) const;
    void RenderShiny(const Cam&, const glm::vec3&) const;
    void RenderSkybox(const Cam&) const;
    void RenderWindows(const Cam&) const;
    void SetUnis(const Cam&, short = 0, const glm::vec3& = glm::vec3(0.f), const glm::vec3& = glm::vec3(1.f), const glm::vec4& = {0.f, 1.f, 0.f, 0.f}) const;
public:
	Scene();
	~Scene();
	void RenderToCreatedFB(const Cam&) const, RenderToDefaultFB(const uint&) const;
};