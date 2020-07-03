#pragma once
#include "Buffer.h"
#include "Cam.h"
#include "Mesh.h"
#include "Model.h"
#include "ShaderProg.h"
#include "SpriteAni.h"

class Scene final{
    glm::vec3 quadPos[5]{
        glm::vec3(1.5f, 1.0f, -10.5f),
        glm::vec3(1.5f, 1.5f, 2.5f),
        glm::vec3(0.0f, 12.f, -5.f),
        glm::vec3(-0.5f, 1.0f, -6.f),
        glm::vec3(0.5f, -1.5f, -1.5f)
    };
    std::vector<cstr> texFaces{
        "Resources/Textures/Skyboxes/Right.png",
        "Resources/Textures/Skyboxes/Left.png",
        "Resources/Textures/Skyboxes/Top.png",
        "Resources/Textures/Skyboxes/Bottom.png",
        "Resources/Textures/Skyboxes/Front.png",
        "Resources/Textures/Skyboxes/Back.png"
    };
	Mesh* meshes[5];
    Model* models[14];
	ShaderProg* basicShaderProg;
	ShaderProg* explosionShaderProg;
	ShaderProg* outlineShaderProg;
	ShaderProg* normalsShaderProg;
	ShaderProg* quadShaderProg;
	ShaderProg* screenQuadShaderProg;
    SpriteAnimation* spriteAni;
    Tex cubemap;
    UniBuffer* magnitudeStorer;
    UniBuffer* brightnessStorer;
    void DrawInstance(const Cam&, const bool&, const glm::vec3&, const glm::vec3&) const;
    void RenderCampfire(const Cam& cam) const;
    void RenderNormals(const Cam&, const bool&) const;
    void RenderShiny(const Cam&, const glm::vec3&, const glm::vec3&, bool) const;
    void RenderSky(const Cam&, const bool&&) const;
    void RenderTreesAndRocks(const Cam& cam) const;
    void RenderWindows(const Cam&) const;
    void SetUnis(const Cam&, short = 0, const glm::vec3& = glm::vec3(0.f), const glm::vec4& = {0.f, 1.f, 0.f, 0.f}, const glm::vec3& = glm::vec3(1.f)) const;
public:
	Scene();
	~Scene();
    void Init();
    void Update(Cam const&);
    void RenderToCreatedFB(Cam const&, const Tex* const&, const uint* const&);
    void RenderToDefaultFB(const Tex&, const int&, const glm::vec3& = glm::vec3(0.f), const glm::vec3& = glm::vec3(1.f)) const;
};