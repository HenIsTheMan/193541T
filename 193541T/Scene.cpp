#include "Scene.h"
#include "LightChief.h"

extern float angularFOV;

Scene::Scene():
	meshes{new Mesh(LoadQuadVertices(1.f), {0, 1, 2, 0, 3, 1}), new Mesh(LoadCubeVertices(), {0}), new Mesh(LoadPtVertices(), {0}), new Mesh(LoadQuadVertices(.05f), {0, 1, 2, 0, 3, 1}), new Mesh(LoadQuadVertices(1.f), {0, 1, 2, 0, 3, 1}),
    new Mesh(LoadQuadVertices(1.f), {0, 1, 2, 0, 3, 1})},
    models{new Model("Resources/Models/Aloe_plant_SF.obj"), new Model("Resources/Models/nanosuit.obj"), new Model("Resources/Models/MyPlanet.obj"), new Model("Resources/Models/rock.obj")},
    basicShaderProg(new ShaderProg("Resources/Shaders/Basic.vs", "Resources/Shaders/Basic.fs")),
    explosionShaderProg(new ShaderProg("Resources/Shaders/Basic.vs", "Resources/Shaders/Basic.fs", "Resources/Shaders/Explosion.gs")),
    outlineShaderProg(new ShaderProg("Resources/Shaders/Basic.vs", "Resources/Shaders/Outline.fs")),
    normalsShaderProg(new ShaderProg("Resources/Shaders/Basic.vs", "Resources/Shaders/Outline.fs", "Resources/Shaders/Normals.gs")),
    quadShaderProg(new ShaderProg("Resources/Shaders/Basic.vs", "Resources/Shaders/Quad.fs")),
    screenQuadShaderProg(new ShaderProg("Resources/Shaders/Basic.vs", "Resources/Shaders/ScreenQuad.fs")),
    simpleDepthShaderProg(new ShaderProg("Resources/Shaders/SimpleDepth.vs", "Resources/Shaders/SimpleDepth.fs")),
    cubemapRefID(CreateCubemap(texFaces))
{
    meshes[0]->LoadTexture("Resources/Textures/blending_transparent_window.png", "d"); //Issues when too close to each other??
    meshes[1]->LoadTexture("Resources/Textures/container2.png", "d"); //UseTex(...) for repeated texs of meshes and models??
    //meshes[1]->LoadTexture("Resources/Textures/container2_specular.png", "s");
    //meshes[1]->LoadTexture("Resources/Textures/matrix.jpg", "e");
    meshes[4]->LoadTexture("Resources/Textures/wood.png", "d");
    meshes[5]->LoadTexture("Resources/Textures/brickwall.jpg", "d");
    meshes[5]->LoadTexture("Resources/Textures/brickwall_normal.jpg", "n");

    //for(float i = 0.f; i < 5.f; ++i){ //Point light can cover a dist of 50 (from given table)
    //    LightChief::CreateLightP(glm::vec3(5.f * i), 1.f, .09f, .032f);
    //}
    //LightChief::CreateLightD(glm::vec3(0.f, -1.f, 0.f));
    LightChief::CreateLightS(glm::vec3(0.f), glm::vec3(0.f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)));
}

Scene::~Scene(){
    for(short i = 0; i < sizeof(meshes) / sizeof(meshes[0]); ++i){
        delete meshes[i];
    }
    for(short i = 0; i < sizeof(models) / sizeof(models[0]); ++i){
        delete models[i];
    }
    delete basicShaderProg;
    delete explosionShaderProg;
    delete outlineShaderProg;
    delete normalsShaderProg;
    delete quadShaderProg;
    delete screenQuadShaderProg;
    delete simpleDepthShaderProg;
}

const std::vector<Vertex> Scene::LoadQuadVertices(const float&& NDC) const{ //Actual winding order is calculated at the rasterization stage after the vertex shader has run //Vertices are then seen from the viewer's POV
    glm::vec3 pos0(-NDC, -NDC, 0.f);
    glm::vec3 pos1(NDC, NDC, 0.f);
    glm::vec3 pos2(-NDC, NDC, 0.f);
    glm::vec3 pos3(NDC, -NDC, 0.f);
    glm::vec2 uv0(0.f, 0.f);
    glm::vec2 uv1(1.f, 1.f);
    glm::vec2 uv2(0.f , 1.f);
    glm::vec2 uv3(1.f, 0.f);

    glm::vec3 edge1 = pos1 - pos0;
    glm::vec3 edge2 = pos2 - pos0;
    glm::vec2 deltaUV1 = uv1 - uv0;
    glm::vec2 deltaUV2 = uv2 - uv0;
    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    glm::vec3 tangent1, bitangent1;
    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent1 = glm::normalize(tangent1);

    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent1 = glm::normalize(bitangent1);
    
    std::vector<Vertex> vertices;
    vertices.emplace_back(Vertex(glm::vec3(-NDC, -NDC, 0.f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.0f, 0.0f, 1.f), tangent1, bitangent1));
    vertices.emplace_back(Vertex(glm::vec3(NDC, NDC, 0.f), glm::vec4(0.f, 1.f, 1.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(0.0f, 0.0f, 1.f), tangent1, bitangent1));
    vertices.emplace_back(Vertex(glm::vec3(-NDC, NDC, 0.f), glm::vec4(1.f, 0.f, 0.f, 1.f), glm::vec2(0.f, 1.f), glm::vec3(0.0f, 0.0f, 1.f), tangent1, bitangent1));
    vertices.emplace_back(Vertex(glm::vec3(NDC, -NDC, 0.f), glm::vec4(0.f, 1.f, 0.f, 1.f), glm::vec2(1.f, 0.f), glm::vec3(0.0f, 0.0f, 1.f), tangent1, bitangent1));
    return vertices;
}

const std::vector<Vertex> Scene::LoadCubeVertices() const{ //In local space (local to mesh)
    std::vector<Vertex> vertices;

    ///Specify vertices in CCW winding order so front faces are rendered in CW order while...
    vertices.emplace_back(Vertex(glm::vec3(-1.f, -1.f, -1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.f, 1.f, -1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.f, -1.f, -1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.f, 1.f, -1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(-1.f, -1.f, -1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(-1.f, 1.f, -1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));

    vertices.emplace_back(Vertex(glm::vec3(-1.f, -1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.f, -1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.f, 1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.f, 1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(-1.f, 1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(-1.f, -1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

    vertices.emplace_back(Vertex(glm::vec3(-1.f, 1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(-1.f, 1.f, -1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(-1.f, -1.f, -1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(-1.f, -1.f, -1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(-1.f, -1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(-1.f, 1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));

    vertices.emplace_back(Vertex(glm::vec3(1.f, 1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.f, -1.f, -1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.f, 1.f, -1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.f, -1.f, -1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.f, 1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.f, -1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

    vertices.emplace_back(Vertex(glm::vec3(-1.f, -1.f, -1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.f, -1.f, -1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.f, -1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.f, -1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(-1.f, -1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(-1.f, -1.f, -1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

    vertices.emplace_back(Vertex(glm::vec3(-1.f, 1.f, -1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.f, 1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.f, 1.f, -1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.f, 1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(-1.f, 1.f, -1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    vertices.emplace_back(Vertex(glm::vec3(-1.f, 1.f, 1.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    return vertices;
}

const std::vector<Vertex> Scene::LoadPtVertices() const{ //Draw 5 pts with colour on the z-plane in NDC??
    std::vector<Vertex> vertices;
    vertices.emplace_back(Vertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 0.0f), glm::vec3(0.f, 0.f, 0.f)));
    vertices.emplace_back(Vertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)));
    vertices.emplace_back(Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)));
    vertices.emplace_back(Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)));
    vertices.emplace_back(Vertex(glm::vec3(0.f, 0.f, 0.f), glm::vec4(1.f, 0.f, 1.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)));
    return vertices;
}

const uint Scene::CreateCubemap(const std::vector<cstr>& faces) const{
    uint cubeMap;
    glGenTextures(1, &cubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap); {
        int width, height, nrChannels;
        unsigned char* data;
        for(GLuint i = 0; i < faces.size(); ++i){ //Gen a tex for each face of the currently bound cubemap
            data = stbi_load(faces[i], &width, &height, &nrChannels, 0);
            if(data){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); //All tex targets are looped through this way as the BTS int value of the enums is linearly incremented
            } else{
                printf("Cubemap tex failed to load at path: %s\n", faces[i]);
            }
            stbi_image_free(data);
        }
        ///TexCoords exactly between 2 adj faces of the cubemap might not hit an exact face due to hardware limitations so use GL_CLAMP_TO_EDGE to tell OpenGL to return their edge values when tex sampling between faces
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return cubeMap;
}

void Scene::DrawInstance(const Cam& cam, const bool& type, const glm::vec3& translate = glm::vec3(0.f), const glm::vec3& scale = glm::vec3(1.f)) const{
    SetUnis(cam, 2 * (scale == glm::vec3(1.f)), translate, {0.f, 1.f, 0.f, 0.f}, scale);
    (type ? models[1]->Draw(1, scale == glm::vec3(1.f)) : meshes[1]->Draw(0, scale == glm::vec3(1.f)));
}

void Scene::Update(Cam const& cam){
    if(LightChief::sLights.size()){
        LightChief::sLights[0].pos = cam.GetPos();
        LightChief::sLights[0].dir = cam.CalcFront();
    }
}

void Scene::RenderNormals(const Cam& cam, const bool& type) const{ //Can use to add fur //Wrong normals due to incorrectly loading vertex data, improperly specifying vertex attributes or incorrectly managing them in the shaders
    normalsShaderProg->Use();
    ShaderProg::SetUni1f("len", .5f);
    ShaderProg::SetUni3f("myRGB", 1.f, 1.f, 0.f);
    ShaderProg::SetUni1i("drawNormals", 1);
    SetUnis(cam, 0);
    (type ? models[1]->Draw(1, 0) : meshes[1]->Draw(0, 0));
    ShaderProg::SetUni1i("drawNormals", 0);
}

void Scene::RenderStuff(const Cam& cam) const{
    basicShaderProg->Use();
    const uint amt = 2000;
    SetUnis(cam, 2, glm::vec3(0.f, 10.f, 0.f));
    models[2]->Draw(1, 1);
    ShaderProg::SetUni1i("useMat", 1);
    models[3]->DrawInstanced(1, 1, amt);
    ShaderProg::SetUni1i("useMat", 0);
    SetUnis(cam, 0);
    ShaderProg::SetUni1i("useFlatColour", 1); {
        ShaderProg::SetUni1i("useOffset", 1); {
            meshes[3]->DrawInstanced(1, 0, 100);
        } ShaderProg::SetUni1i("useOffset", 0);
        glPointSize(50.f);
        glLineWidth(2.f);
        meshes[2]->DrawPts(5);
    } ShaderProg::SetUni1i("useFlatColour", 0);
}

void Scene::RenderShiny(const Cam& cam, const glm::vec3& translate = glm::vec3(0.f), const glm::vec3& scale = glm::vec3(1.f), bool mergeBorders = 1) const{
    //Default is glStencilFunc(GL_ALWAYS, 1, 0xFF)
    glStencilMask(0xFF); //Set bitmask that is ANDed with stencil value abt to be written to stencil buffer //Each bit is written to the stencil buffer unchanged (bitmask of all 1s [default])

    basicShaderProg->Use();
    ShaderProg::SetUni1i("reflection", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapRefID); {
        ShaderProg::SetUni1i("cubemapSampler", 1);
        DrawInstance(cam, 0, translate);
    } glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
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

void Scene::RenderSkybox(const Cam& cam) const{
    ///Fragment shader runs for each pixel if skybox renders 1st although only some parts of it eventually becomes visible //Use early depth test to save bandwidth
    ///Render skybox last so depth buffer is filled with depth values of opaque objs and hence call fragment shader to render only fragments of the skybox which pass the early depth test
    //Need glDepthMask (perform depth test on all fragments but not update the depth buffer [use read-only depth buffer] if GL_FALSE) if skybox drawn as 1st obj as it's only 1x1x1
    //Skybox is likely to fail most depth tests and hence fail to render as it's 1x1x1 but cannot just disable depth test as skybox will overwrite all opaque objs
    glDepthFunc(GL_LEQUAL); //Modify comparison operators used for depth test //Depth buffer filled with 1.0s for the skybox so must ensure the skybox passes the depth tests with depth values <= that in the depth buffer??
    glFrontFace(GL_CW);
    //glDepthMask(GL_FALSE);

    basicShaderProg->Use();
    ShaderProg::SetUni1i("cubemap", 1);
    SetUnis(cam, 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapRefID); {
        ShaderProg::SetUni1i("cubemapSampler", 0);
        meshes[1]->Draw(0, 0);
    } glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    ShaderProg::SetUni1i("cubemap", 0);

    //glDepthMask(GL_TRUE);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LESS);
}

void Scene::RenderWindows(const Cam& cam) const{
    ///Draw opaque objs first so dst colour used in blend eqn is correct
    std::map<float, glm::vec3> sorted; //DS from STL lib which sorts its values based on its keys //Does not take rotations and scaling into acct and weirdly shaped objs need a diff metric than simply a pos vector
    for(uint i = 0; i < sizeof(quadPos) / sizeof(quadPos[0]); ++i){ //Transparent parts are written to the depth buffer like any other value so they are depth-tested like any other opaque obj would be (depth test discards other parts behind transparent parts)
        float dist = glm::length(cam.GetPos() - quadPos[i]);
        sorted[dist] = quadPos[i];
    }

    quadShaderProg->Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, meshes[0]->textures[0].refID); {
        ShaderProg::SetUni1i("texSampler", 0);
        for(std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it){
            SetUnis(cam, 0, it->second);
            meshes[0]->Draw(1, 0);
        }
    } glBindTexture(GL_TEXTURE_2D, 0);
}

void Scene::RenderToCreatedFB(Cam const& cam, const uint* const& enCubemap, const uint* const& depthTexs) const{ //Intermediate results passed between framebuffers to remain in linear space and only the last framebuffer applies gamma correction before being sent to the monitor??
    //glStencilMask(0x00); //Make outline overlap

    if(!depthTexs){
        basicShaderProg->Use();
        ShaderProg::SetUni1i("depthOnly", 1);
        explosionShaderProg->Use();
        ShaderProg::SetUni1i("depthOnly", 1);
    } else{
        Cam dCam = Cam(glm::vec3(-2.f, 4.f, -1.f), glm::vec3(0.f));
        glm::mat4 dLightSpaceVP = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 7.5f) * dCam.LookAt(); //Ensure projection frustum size is correct so no fragments of objs are clipped (fragments of objs not in the depth/... map will not produce shadows)
        glm::mat4 sLightSpaceVP = glm::perspective(glm::radians(angularFOV), 1024.f / 1024.f, 1.f, 50.f) * cam.LookAt(); //Depth is non-linear with perspective projection
        //glm::length(cam.GetPos()) / 50.f, glm::length(cam.GetPos()) * 20.f

        basicShaderProg->Use();
        ShaderProg::SetUni1i("depthOnly", 0);
        ShaderProg::SetUni1i("showShadows", 0);
        ShaderProg::SetUniMtx4fv("dLightSpaceVP", glm::value_ptr(dLightSpaceVP));
        ShaderProg::SetUniMtx4fv("sLightSpaceVP", glm::value_ptr(sLightSpaceVP));
        for(short i = 0; i < 2; ++i){
            glActiveTexture(GL_TEXTURE30 + i);
            glBindTexture(GL_TEXTURE_2D, depthTexs[i]);
            ShaderProg::SetUni1i(~i & 1 ? "shadowMapD" : "shadowMapS", 30 + i);
        }
        explosionShaderProg->Use();
        ShaderProg::SetUni1i("depthOnly", 0);
        ShaderProg::SetUni1i("showShadows", 0);
        ShaderProg::SetUniMtx4fv("dLightSpaceVP", glm::value_ptr(dLightSpaceVP));
        ShaderProg::SetUniMtx4fv("sLightSpaceVP", glm::value_ptr(sLightSpaceVP));
        for(short i = 0; i < 2; ++i){
            glActiveTexture(GL_TEXTURE30 + i);
            glBindTexture(GL_TEXTURE_2D, depthTexs[i]);
            ShaderProg::SetUni1i(~i & 1 ? "shadowMapD" : "shadowMapS", 30 + i);
        }
    }

    //++ DrawQuads(...) and DrawCubes(...)
    RenderStuff(cam);
    if(enCubemap){
        explosionShaderProg->Use();
        ShaderProg::SetUni1f("magnitude", 1.3f);
        ShaderProg::SetUni1f("time", (float)glfwGetTime());
        ShaderProg::SetUni1i("reflection", 1);
        ShaderProg::SetUni1i("emission", 1);
        ShaderProg::SetUni1i("explosion", 1);
        glActiveTexture(GL_TEXTURE3); //New auto way?? //Why 2 cannot for nanosuit??
        ShaderProg::SetUni1i("cubemapSampler", 3); //New placement??
        glBindTexture(GL_TEXTURE_CUBE_MAP, *enCubemap);
        DrawInstance(cam, 0); //Pass func ptr into another func??
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        ShaderProg::SetUni1i("explosion", 0);
        ShaderProg::SetUni1i("emission", 0);
        ShaderProg::SetUni1i("reflection", 0);
    }
    RenderNormals(cam, 0);
    for(short i = 0; i < 5; ++i){
        RenderShiny(cam, quadPos[i] + glm::vec3(0.f, -5.f, 0.f), glm::vec3(1.25f), 0); //Model outline??
    }

    basicShaderProg->Use();
    SetUnis(cam, 2, {0.f, -12.f, 0.f}, glm::vec4(1.f, 0.f, 0.f, -90.f), glm::vec3(25.f));
    meshes[4]->Draw(1, 1);
    ShaderProg::SetUni1i("bump", 1);
    SetUnis(cam, 2, {0.f, 0.f, -20.f}, glm::vec4(1.f, 0.f, 0.f, -90.f), glm::vec3(5.f));
    meshes[5]->Draw(1, 1);
    ShaderProg::SetUni1i("bump", 0);

    RenderSkybox(cam);
    RenderWindows(cam);
}

void Scene::RenderToDefaultFB(const uint& texColourBuffer, const glm::vec3& translate, const glm::vec3& scale) const{
    screenQuadShaderProg->Use();
    ShaderProg::SetUni1i("screenQuad", 1);
    glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.f), translate), scale);
    ShaderProg::SetUniMtx4fv("model", glm::value_ptr(model), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texColourBuffer); {
        ShaderProg::SetUni1i("screenTex", 0);
        meshes[0]->Draw(1, 0);
    } glBindTexture(GL_TEXTURE_2D, 0);
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
        projection = glm::perspective(glm::radians(angularFOV), cam.GetAspectRatio(), .1f, 100.f); //aspect ratio?? //++ long dist from near plane to...??
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