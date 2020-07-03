#include "Scene.h"
#include "LightChief.h"

extern float angularFOV;

Scene::Scene():
    meshes{Mesh::CreatePts(), Mesh::CreateQuad(), Mesh::CreateCube(), Mesh::CreateRandHeightMap(Mesh::HillAlgParams(1000, 1000, 50, 300, 400, .01f, .02f))}, //Mesh::CreateHeightMap("Resources/Textures/HeightMaps/heightmap.raw")
    models{new Model("Resources/Models/Aloe_plant_SF.obj"), new Model("Resources/Models/nanosuit.obj"),
    new Model("Resources/Models/MyPlanet.obj"), new Model("Resources/Models/rock.obj"), new Model("Resources/Models/Skydome.obj")},
    basicShaderProg(new ShaderProg("Resources/Shaders/Basic.vs", "Resources/Shaders/Basic.fs")),
    explosionShaderProg(new ShaderProg("Resources/Shaders/Basic.vs", "Resources/Shaders/Basic.fs", "Resources/Shaders/Explosion.gs")),
    outlineShaderProg(new ShaderProg("Resources/Shaders/Basic.vs", "Resources/Shaders/Outline.fs")),
    normalsShaderProg(new ShaderProg("Resources/Shaders/Basic.vs", "Resources/Shaders/Outline.fs", "Resources/Shaders/Normals.gs")),
    quadShaderProg(new ShaderProg("Resources/Shaders/Basic.vs", "Resources/Shaders/Quad.fs")),
    screenQuadShaderProg(new ShaderProg("Resources/Shaders/Basic.vs", "Resources/Shaders/ScreenQuad.fs"))
{
    Init();
}

Scene::~Scene(){
    for(short i = 0; i < sizeof(meshes) / sizeof(meshes[0]); ++i){
        delete meshes[i];
    }
    for(short i = 0; i < sizeof(models) / sizeof(models[0]); ++i){
        delete models[i];
    }
    cubemap.Del();
    delete magnitudeStorer;
    delete brightnessStorer;
    delete basicShaderProg;
    delete explosionShaderProg;
    delete outlineShaderProg;
    delete normalsShaderProg;
    delete quadShaderProg;
    delete screenQuadShaderProg;
}

void Scene::DrawInstance(const Cam& cam, const bool& type, const glm::vec3& translate = glm::vec3(0.f), const glm::vec3& scale = glm::vec3(1.f)) const{
    SetUnis(cam, 2 * (scale == glm::vec3(1.f)), translate, {0.f, 1.f, 0.f, 0.f}, scale);
    (type ? models[1]->Draw(GL_TRIANGLES, 1, scale == glm::vec3(1.f)) : meshes[2]->Draw(GL_TRIANGLES, 1));
}

void Scene::RenderNormals(const Cam& cam, const bool& type) const{ //Can use to add fur //Wrong normals due to incorrectly loading vertex data, improperly specifying vertex attributes or incorrectly managing them in the shaders
    normalsShaderProg->Use();
    ShaderProg::SetUni1f("len", .5f);
    ShaderProg::SetUni3f("myRGB", 1.f, 1.f, 0.f);
    ShaderProg::SetUni1i("drawNormals", 1);
    SetUnis(cam, 0);
    (type ? models[0]->Draw(GL_TRIANGLES, 1, 0) : meshes[2]->Draw(GL_TRIANGLES, 1));
    ShaderProg::SetUni1i("drawNormals", 0);
}

void Scene::RenderStuff(const Cam& cam) const{
    basicShaderProg->Use();
    const uint amt = 1000;
    SetUnis(cam, 2, glm::vec3(0.f, 20.f, 0.f));
    models[2]->Draw(GL_TRIANGLES, 1, 1);
    ShaderProg::SetUni1i("useMat", 1);
    models[3]->Draw(GL_TRIANGLES, amt, 1);
    ShaderProg::SetUni1i("useMat", 0);
    ShaderProg::SetUni1i("useFlatColour", 1); {
        ShaderProg::SetUni1i("useOffset", 1); {
            SetUnis(cam, 0, glm::vec3(0.f, 0.f, -18.f), glm::vec4(0.f, 1.f, 0.f, 0.f), glm::vec3(.5f));
            meshes[1]->Draw(GL_TRIANGLES, 100);
        } ShaderProg::SetUni1i("useOffset", 0);
        SetUnis(cam, 0, glm::vec3(0.f, 0.f, -8.f));
        meshes[0]->Draw(GL_POINTS, 5);
    } ShaderProg::SetUni1i("useFlatColour", 0);
}

void Scene::RenderShiny(const Cam& cam, const glm::vec3& translate = glm::vec3(0.f), const glm::vec3& scale = glm::vec3(1.f), bool mergeBorders = 1) const{
    //Default is glStencilFunc(GL_ALWAYS, 1, 0xFF)
    //glStencilMask(0xFF); //Set bitmask that is ANDed with stencil value abt to be written to stencil buffer //Each bit is written to the stencil buffer unchanged (bitmask of all 1s [default])

    basicShaderProg->Use();
    ShaderProg::SetUni1i("reflection", 1);
    ShaderProg::SetUni1i("useReflectionMap", 0);
    ShaderProg::SetUni1i("emission", 1);
    ShaderProg::UseTex(GL_TEXTURE_CUBE_MAP, cubemap, "cubemapSampler");
    meshes[2]->textures[2].SetActiveOnMesh(1);
    DrawInstance(cam, 0, translate);
    ShaderProg::StopUsingTex(GL_TEXTURE_CUBE_MAP, cubemap);
    ShaderProg::SetUni1i("emission", 0);
    ShaderProg::SetUni1i("useReflectionMap", 0);
    ShaderProg::SetUni1i("reflection", 0);

    if(mergeBorders){
        glDepthMask(GL_FALSE);
    }
    if(scale != glm::vec3(1.f)){
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF); //The frag passes... and is drawn if its ref value of 1 is not equal to stencil value in the stencil buffer //++params??
        outlineShaderProg->Use();
        ShaderProg::SetUni3f("myRGB", 2.1f, .7f, 1.3f);
        DrawInstance(cam, 0, translate, scale);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
    }
    if(mergeBorders){
        glDepthMask(GL_TRUE);
    } else{
        glClear(GL_STENCIL_BUFFER_BIT);
    }
}

void Scene::RenderSky(const Cam& cam, const bool&& type) const{
    if(type){
        basicShaderProg->Use();
        ShaderProg::SetUni1i("skydome", 1);
        ShaderProg::LinkUniBlock("Settings", 1);
        glCullFace(GL_FRONT);
        SetUnis(cam, 1, glm::vec3(0.f, -20.f, 0.f), glm::vec4(0.f, 1.f, 0.f, 0.f), glm::vec3(1.f, -1.5f, 1.f));
        models[4]->Draw(GL_TRIANGLES, 1, 1);
        glCullFace(GL_BACK);
        SetUnis(cam, 1, glm::vec3(0.f, -40.f, 0.f), glm::vec4(0.f, 1.f, 0.f, 0.f), glm::vec3(1.f, 1.5f, 1.f));
        models[4]->Draw(GL_TRIANGLES, 1, 1);
        ShaderProg::SetUni1i("skydome", 0);
    } else{
        glDepthFunc(GL_LEQUAL); //Modify comparison operators used for depth test //Depth buffer filled with 1.0s for the skybox so must ensure the skybox passes the depth tests with depth values <= that in the depth buffer??
        glFrontFace(GL_CW);
        //glDepthMask(GL_FALSE); //Need glDepthMask (perform depth test on all fragments but not update the depth buffer [use read-only depth buffer] if GL_FALSE) if skybox drawn as 1st obj as it's only 1x1x1

        basicShaderProg->Use();
        SetUnis(cam, 1);
        ShaderProg::SetUni1i("cubemap", 1);
        ShaderProg::UseTex(GL_TEXTURE_CUBE_MAP, cubemap, "cubemapSampler");
        meshes[2]->Draw(GL_TRIANGLES, 1);
        ShaderProg::StopUsingTex(GL_TEXTURE_CUBE_MAP, cubemap);
        ShaderProg::SetUni1i("cubemap", 0);

        //glDepthMask(GL_TRUE); //Skybox is likely to fail most depth tests and hence fail to render as it's 1x1x1 but cannot just disable depth test as skybox will overwrite all opaque objs
        glFrontFace(GL_CCW);
        glDepthFunc(GL_LESS);
    }
}

void Scene::RenderWindows(const Cam& cam) const{
    std::map<float, glm::vec3> sorted; //DS from STL lib which sorts its values based on its keys //Does not take rotations and scaling into acct and weirdly shaped objs need a diff metric than simply a pos vector
    for(uint i = 0; i < sizeof(quadPos) / sizeof(quadPos[0]); ++i){ //Transparent parts are written to the depth buffer like any other value so they are depth-tested like any other opaque obj would be (depth test discards other parts behind transparent parts)
        float dist = glm::length(cam.GetPos() - quadPos[i]);
        sorted[dist] = quadPos[i];
    }

    quadShaderProg->Use();
    ShaderProg::UseTex(GL_TEXTURE_2D, meshes[1]->textures[0], "texSampler");
    for(std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it){
        SetUnis(cam, 0, it->second);
        meshes[1]->Draw(GL_TRIANGLES, 1);
    }
    ShaderProg::StopUsingTex(GL_TEXTURE_2D, meshes[1]->textures[0]);
}

void Scene::Init(){
    cubemap.Create(GL_TEXTURE_CUBE_MAP, 999, 0, 0, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, "skybox", &texFaces, 0);
    meshes[1]->LoadTex("Resources/Textures/blending_transparent_window.png", "d");
    meshes[1]->LoadTex("Resources/Textures/wood.png", "d");
    meshes[1]->LoadTex("Resources/Textures/brickwall.jpg", "d");
    meshes[1]->LoadTex("Resources/Textures/brickwall_normal.jpg", "n");
    meshes[2]->LoadTex("Resources/Textures/container2.png", "d");
    meshes[2]->LoadTex("Resources/Textures/container2_specular.png", "s");
    meshes[2]->LoadTex("Resources/Textures/awesomeface.png", "e");
    meshes[3]->LoadTex("Resources/Textures/grass.jpg", "d");

    size_t size = meshes[2]->textures.size();
    for(size_t i = 0; i < size; ++i){ //?????????????????
        meshes[2]->textures[i].SetActiveOnMesh(1);
    }

    glPointSize(50.f);
    glLineWidth(2.f);
    //for(float i = 0.f; i < 5.f; ++i){ //Point light can cover a dist of 50 (from given table)
    //    LightChief::CreateLightP(glm::vec3(5.f * i), 1.f, .09f, .032f);
    //}
    //LightChief::CreateLightD(glm::vec3(0.f, -1.f, 0.f));
    LightChief::CreateLightS(glm::vec3(0.f), glm::vec3(0.f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)));

    magnitudeStorer = new UniBuffer(1.3f, 0);
    brightnessStorer = new UniBuffer(.7f, 1);
}

void Scene::Update(Cam const& cam){
    if(LightChief::sLights.size()){
        LightChief::sLights[0].pos = cam.GetPos();
        LightChief::sLights[0].dir = cam.CalcFront();
    }
}

void Scene::RenderToCreatedFB(Cam const& cam, const Tex* const& enCubemap, const uint* const& depthTexs){ //Intermediate results passed between framebuffers to remain in linear space and only the last framebuffer applies gamma correction before being sent to the monitor??
    //glStencilMask(0x00); //Make outline overlap

    //if(!depthTexs){
    //    basicShaderProg->Use();
    //    ShaderProg::SetUni1i("depthOnly", 1);
    //    explosionShaderProg->Use();
    //    ShaderProg::SetUni1i("depthOnly", 1);
    //} else{
    //    Cam dCam = Cam(glm::vec3(-2.f, 4.f, -1.f), glm::vec3(0.f));
    //    glm::mat4 dLightSpaceVP = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 7.5f) * dCam.LookAt(); //Ensure projection frustum size is correct so no fragments of objs are clipped (fragments of objs not in the depth/... map will not produce shadows)
    //    glm::mat4 sLightSpaceVP = glm::perspective(glm::radians(angularFOV), 1024.f / 1024.f, 1.f, 50.f) * cam.LookAt(); //Depth is non-linear with perspective projection
    //    //glm::length(cam.GetPos()) / 50.f, glm::length(cam.GetPos()) * 20.f

    //    basicShaderProg->Use();
    //    ShaderProg::SetUni1i("depthOnly", 0);
    //    ShaderProg::SetUni1i("showShadows", 0);
    //    ShaderProg::SetUniMtx4fv("dLightSpaceVP", glm::value_ptr(dLightSpaceVP));
    //    ShaderProg::SetUniMtx4fv("sLightSpaceVP", glm::value_ptr(sLightSpaceVP));
    //    //for(short i = 0; i < 2; ++i){ //why reflect red??
    //    //    ShaderProg::UseTex(GL_TEXTURE_2D, depthTexs[i], ~i & 1 ? "shadowMapD" : "shadowMapS");
    //    //}
    //    explosionShaderProg->Use();
    //    ShaderProg::SetUni1i("depthOnly", 0);
    //    ShaderProg::SetUni1i("showShadows", 0);
    //    ShaderProg::SetUniMtx4fv("dLightSpaceVP", glm::value_ptr(dLightSpaceVP));
    //    ShaderProg::SetUniMtx4fv("sLightSpaceVP", glm::value_ptr(sLightSpaceVP));
    //    //for(short i = 0; i < 2; ++i){ //why reflect red??
    //    //    ShaderProg::UseTex(GL_TEXTURE_2D, depthTexs[i], ~i & 1 ? "shadowMapD" : "shadowMapS");
    //    //}
    //}

    RenderStuff(cam);
    if(enCubemap){
        explosionShaderProg->Use();
        ShaderProg::LinkUniBlock("ExplosionUnis", 0);
        ShaderProg::SetUni1f("time", (float)glfwGetTime());
        ShaderProg::SetUni1i("reflection", 1);
        ShaderProg::SetUni1i("useReflectionMap", 0);
        ShaderProg::SetUni1i("explosion", 1);
        ShaderProg::SetUni1i("bump", 0);
        ShaderProg::UseTex(GL_TEXTURE_CUBE_MAP, *enCubemap, "cubemapSampler");
        DrawInstance(cam, 0, glm::vec3(0.f), glm::vec3(5.f));
        //DrawInstance(cam, 1); //scale up cube to see reflections better??
        ShaderProg::StopUsingTex(GL_TEXTURE_CUBE_MAP, *enCubemap);
        ShaderProg::SetUni1i("bump", 0);
        ShaderProg::SetUni1i("explosion", 0);
        ShaderProg::SetUni1i("useReflectionMap", 0);
        ShaderProg::SetUni1i("reflection", 0);
    }

    RenderNormals(cam, 0);
    for(short i = 0; i < 5; ++i){
        RenderShiny(cam, quadPos[i] + glm::vec3(0.f, -5.f, 0.f), glm::vec3(1.25f), 0); //Model outline??
    }

    basicShaderProg->Use();
    SetUnis(cam, 2, {0.f, -12.f, 0.f}, glm::vec4(1.f, 0.f, 0.f, -90.f), glm::vec3(25.f));
    meshes[1]->textures[1].SetActiveOnMesh(1);
    meshes[1]->Draw(GL_TRIANGLES, 1);
    ShaderProg::SetUni1i("bump", 1);
    SetUnis(cam, 2, {0.f, 0.f, -20.f}, glm::vec4(1.f, 0.f, 0.f, -90.f), glm::vec3(5.f));
    meshes[1]->textures[2].SetActiveOnMesh(1);
    meshes[1]->textures[3].SetActiveOnMesh(1);
    meshes[1]->Draw(GL_TRIANGLES, 1);
    ShaderProg::SetUni1i("bump", 0);

    basicShaderProg->Use();
    SetUnis(cam, 2, glm::vec3(0.f, -40.f, 0.f), glm::vec4(1.f, 0.f, 0.f, 0.f), glm::vec3(100.f));
    meshes[3]->textures[0].SetActiveOnMesh(1);
    meshes[3]->Draw(GL_TRIANGLE_STRIP, 1);

    RenderSky(cam, 1); //Draw opaque objs first so depth buffer is filled with depth values of opaque objs and hence call frag shader to render only frags which pass the early depth test (saves bandwidth as frag shader does not run for frags that fail early depth test)
    RenderWindows(cam); //Draw opaque objs first so dst colour used in blend eqn is correct //Issues when too close to each other??
    ShaderProg::StopUsingAllTexs();
}

void Scene::RenderToDefaultFB(const Tex& texColourBuffer, const int& typePPE, const glm::vec3& translate, const glm::vec3& scale) const{
    screenQuadShaderProg->Use();
    glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.f), translate), scale);
    ShaderProg::SetUniMtx4fv("model", glm::value_ptr(model), 0);
    ShaderProg::SetUni1i("screenQuad", 1);
    ShaderProg::SetUni1i("typePPE", typePPE);
    ShaderProg::UseTex(GL_TEXTURE_2D, texColourBuffer, "screenTex");
    meshes[1]->Draw(GL_TRIANGLES, 1);
    ShaderProg::StopUsingTex(GL_TEXTURE_2D, texColourBuffer);
    ShaderProg::SetUni1i("screenQuad", 0);
}

void Scene::SetUnis(const Cam& cam, short type, const glm::vec3& translate, const glm::vec4& rotate, const glm::vec3& scale) const{
    glm::mat4 model = glm::translate(glm::mat4(1.f), translate);
    model = glm::rotate(model, glm::radians(rotate.w), glm::vec3(rotate));
    model = glm::scale(model, scale);
    glm::mat4 view = (type & 1 ? glm::mat4(glm::mat3(cam.LookAt())) : cam.LookAt()); //Remove translation of skybox in the scene by taking upper-left 3x3 matrix of the 4x4 transformation matrix
    glm::mat4 projection;
    if(cam.GetProjectionType() == 0){
        projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 7.5f); //No perspective deform of vertices of objs in scene as directional light rays are parallel
    } else if(cam.GetProjectionType() == 1){
        projection = glm::perspective(glm::radians(angularFOV), 1024.f / 768.f, glm::length(cam.GetPos()) / 50.f, glm::length(cam.GetPos()) * 20.f); //shadows disappear at high lvls?? //shape distortion??
    } else{
        //projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f); //Ortho projection matrix produces clip coords that are NDC while perspective projection matrix produces clip coords with a range of -w to w
        projection = glm::perspective(glm::radians(angularFOV), cam.GetAspectRatio(), .1f, 9999.f); //++ long dist from near plane to...??
    }
    glm::mat4 MVP = projection * view * model;
    ShaderProg::SetUniMtx4fv("model", glm::value_ptr(model), 0); //Local coords in local/obj space => world coords in world space //SRT
    ShaderProg::SetUniMtx4fv("view", glm::value_ptr(view), 0); //World coords in world space => view coords in view/cam/eye space
    ShaderProg::SetUniMtx4fv("projection", glm::value_ptr(projection), 0); //View coords in view/cam/eye space => clip coords in clip space //Clipped vertices (not in clipping range/vol) are discarded when clipping occurs before frag shaders run
    ShaderProg::SetUniMtx4fv("MVP", glm::value_ptr(MVP), 0);
    ShaderProg::SetUni3f("camPos", cam.GetPos(), 0);

    if(type == 2){
        const size_t &&amtP = LightChief::pLights.size(), &&amtD = LightChief::dLights.size(), &&amtS = LightChief::sLights.size();
        ShaderProg::SetUni1i("amtP", (int)amtP);
        ShaderProg::SetUni1i("amtD", (int)amtD);
        ShaderProg::SetUni1i("amtS", (int)amtS);
        if(amtP){
            for(short i = 0; i < amtP; ++i){
                ShaderProg::SetUni3f(("pLights[" + std::to_string(i) + "].pos").c_str(), LightChief::pLights[i].pos);
                ShaderProg::SetUni1f(("pLights[" + std::to_string(i) + "].constant").c_str(), LightChief::pLights[i].constant);
                ShaderProg::SetUni1f(("pLights[" + std::to_string(i) + "].linear").c_str(), LightChief::pLights[i].linear);
                ShaderProg::SetUni1f(("pLights[" + std::to_string(i) + "].quadratic").c_str(), LightChief::pLights[i].quadratic);
            }
        }
        if(amtD){
            for(short i = 0; i < amtD; ++i){
                ShaderProg::SetUni3f(("dLights[" + std::to_string(i) + "].dir").c_str(), LightChief::dLights[i].dir);
            }
        }
        if(amtS){
            for(short i = 0; i < amtS; ++i){
                ShaderProg::SetUni3f(("sLights[" + std::to_string(i) + "].pos").c_str(), LightChief::sLights[i].pos);
                ShaderProg::SetUni3f(("sLights[" + std::to_string(i) + "].dir").c_str(), LightChief::sLights[i].dir);
                ShaderProg::SetUni1f(("sLights[" + std::to_string(i) + "].cosInnerCutoff").c_str(), LightChief::sLights[i].cosInnerCutoff);
                ShaderProg::SetUni1f(("sLights[" + std::to_string(i) + "].cosOuterCutoff").c_str(), LightChief::sLights[i].cosOuterCutoff);
            }
        }
        ShaderProg::SetUni1f("material.shininess", 32.f); //More light scattering if lower //Test low?? //Make abstract??
    }
}