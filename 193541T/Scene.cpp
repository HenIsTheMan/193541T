#include "Scene.h"

extern float FOV;

Scene::Scene():
	meshes{new Mesh(LoadQuadVertices(), {0, 1, 2, 0, 3, 1}), new Mesh(LoadCubeVertices(), {0}), new Mesh(LoadPtVertices(), {0}), new Mesh(LoadQuadVertices2(), {0, 1, 2, 0, 3, 1})},
    models{new Model("Resources/Models/Aloe_plant_SF.obj"), new Model("Resources/Models/nanosuit.obj"), new Model("Resources/Models/MyPlanet.obj"), new Model("Resources/Models/rock.obj")},
    basicSC(new ShaderChief("Resources/Shaders/Basic.vs", "Resources/Shaders/Basic.fs")),
    explosionSC(new ShaderChief("Resources/Shaders/Basic.vs", "Resources/Shaders/Basic.fs", "Resources/Shaders/Explosion.gs")),
    outlineSC(new ShaderChief("Resources/Shaders/Basic.vs", "Resources/Shaders/Outline.fs")),
    normalsSC(new ShaderChief("Resources/Shaders/Basic.vs", "Resources/Shaders/Outline.fs", "Resources/Shaders/Normals.gs")),
    quadSC(new ShaderChief("Resources/Shaders/Basic.vs", "Resources/Shaders/Quad.fs")),
	screenQuadSC(new ShaderChief("Resources/Shaders/Basic.vs", "Resources/Shaders/ScreenQuad.fs")),
    cubemapRefID(CreateCubemap(texFaces))
{
    meshes[0]->LoadTexture("Resources/Textures/blending_transparent_window.png", "d"); //Issues when too close to each other??
    meshes[1]->LoadTexture("Resources/Textures/container2.png", "d");
    //meshes[1]->LoadTexture("Resources/Textures/container2_specular.png", "s"); //??
    //meshes[1]->LoadTexture("Resources/Textures/matrix.jpg", "e"); //supported??
}

Scene::~Scene(){
    for(short i = 0; i < sizeof(meshes) / sizeof(meshes[0]); ++i){
        delete meshes[i];
    }
    for(short i = 0; i < sizeof(models) / sizeof(models[0]); ++i){
        delete models[i];
    }
    delete basicSC;
    delete explosionSC;
    delete outlineSC;
    delete normalsSC;
    delete quadSC;
    delete screenQuadSC;
}

const std::vector<Vertex> Scene::LoadQuadVertices() const{ //Actual winding order is calculated at the rasterization stage after the vertex shader has run //Vertices are then seen from the viewer's POV
    std::vector<Vertex> vertices;
    vertices.emplace_back(Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
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

const std::vector<Vertex> Scene::LoadQuadVertices2() const{ //2D NDC??
    std::vector<Vertex> vertices;
    vertices.emplace_back(Vertex(glm::vec3(-0.05f, -0.05f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(0.05f, 0.05f, 0.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(-0.05f, 0.05f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(0.05f, -0.05f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
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
    SetUnis(cam, 2 * (scale == glm::vec3(1.f)), translate, scale);
    (type ? models[1]->Draw(1, scale == glm::vec3(1.f)) : meshes[1]->Draw(0, scale == glm::vec3(1.f)));
}

void Scene::RenderStuff(const Cam& cam) const{
    basicSC->UseProg();
    const uint amt = 2000;
    SetUnis(cam, 2, glm::vec3(0.f, 10.f, 0.f));
    models[2]->Draw(1, 1);
    ShaderChief::SetUni1i("useMat", 1);
    models[3]->DrawInstanced(1, 1, amt);
    ShaderChief::SetUni1i("useMat", 0);

    basicSC->UseProg();
    ShaderChief::SetUni1i("useFlatColour", 1); {
        ShaderChief::SetUni1i("useOffset", 1); {
            SetUnis(cam, 0);
            for(short i = 0; i < 100; ++i){
                meshes[3]->DrawInstanced(1, 0, 100);
            }
        } ShaderChief::SetUni1i("useOffset", 0);
        glPointSize(50.f);
        glLineWidth(2.f);
        meshes[2]->DrawPts(5);
    } ShaderChief::SetUni1i("useFlatColour", 0);
}

void Scene::RenderShiny(const Cam& cam, const glm::vec3& translate = glm::vec3(0.f), const glm::vec3& scale = glm::vec3(1.f), bool mergeBorders = 1) const{
    //Default is glStencilFunc(GL_ALWAYS, 1, 0xFF)
    glStencilMask(0xFF); //Set bitmask that is ANDed with stencil value abt to be written to stencil buffer //Each bit is written to the stencil buffer unchanged (bitmask of all 1s [default])

    basicSC->UseProg();
    ShaderChief::SetUni1i("reflection", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapRefID); {
        ShaderChief::SetUni1i("cubemapSampler", 1);
        DrawInstance(cam, 0, translate);
    } glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    ShaderChief::SetUni1i("reflection", 0);

    if(mergeBorders){
        glDepthMask(GL_FALSE);
    }
    if(scale != glm::vec3(1.f)){
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF); //The frag passes... and is drawn if its ref value of 1 is not equal to stencil value in the stencil buffer //++params??
        outlineSC->UseProg();
        ShaderChief::SetUni3f("myRGB", 2.1f, .7f, 1.3f);
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

    basicSC->UseProg();
    ShaderChief::SetUni1i("cubemap", 1);
    SetUnis(cam, 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapRefID); {
        ShaderChief::SetUni1i("cubemapSampler", 0);
        meshes[1]->Draw(0, 0);
    } glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    ShaderChief::SetUni1i("cubemap", 0);

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

    quadSC->UseProg();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, meshes[0]->textures[0].refID); {
        ShaderChief::SetUni1i("texSampler", 0);
        for(std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it){
            SetUnis(cam, 0, it->second);
            meshes[0]->Draw(1, 0);
        }
    } glBindTexture(GL_TEXTURE_2D, 0);
}

void Scene::RenderToCreatedFB(const Cam const& const cam, const uint* const& const enCubemap) const{
    //glStencilMask(0x00); //Make outline overlap

    RenderStuff(cam);

    //++ DrawQuads(...) and DrawCubes(...)

    if(enCubemap){
        explosionSC->UseProg();
        ShaderChief::SetUni1f("magnitude", 2.f);
        ShaderChief::SetUni1f("time", glfwGetTime());
        ShaderChief::SetUni1i("reflection", 0);
        glActiveTexture(GL_TEXTURE3); //Why 2 cannot?? //New auto way??
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapRefID); {
            ShaderChief::SetUni1i("cubemapSampler", 3);
            DrawInstance(cam, 1);
        } glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        ShaderChief::SetUni1i("reflection", 0);

        //Can use to add fur ///Wrong normals due to incorrectly loading vertex data, improperly specifying vertex attributes or incorrectly managing them in the shaders
        normalsSC->UseProg();
        ShaderChief::SetUni1i("drawNormals", 1);
        ShaderChief::SetUni3f("myRGB", 1.f, 1.f, 0.f);
        SetUnis(cam, 0);
        models[1]->Draw(1, 0);
        ShaderChief::SetUni1i("drawNormals", 0);
    }

    for(short i = 0; i < 5; ++i){
        RenderShiny(cam, quadPos[i] + glm::vec3(0.f, -5.f, 0.f), glm::vec3(1.25f), 0);
    }
    RenderSkybox(cam);
    RenderWindows(cam);
}

void Scene::RenderToDefaultFB(const uint& texColourBuffer, const glm::vec3& translate, const glm::vec3& scale) const{
    screenQuadSC->UseProg();
    ShaderChief::SetUni1i("screenQuad", 1);
    glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.f), translate), scale);
    ShaderChief::SetUniMtx4fv("model", glm::value_ptr(model), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texColourBuffer); {
        ShaderChief::SetUni1i("screenTex", 0);
        meshes[0]->Draw(1, 0);
    } glBindTexture(GL_TEXTURE_2D, 0);
}

void Scene::SetUnis(const Cam& cam, short type, const glm::vec3& translate, const glm::vec3& scale, const glm::vec4& rotate) const{
    glm::mat4 model = glm::mat4(1.f);
    model = glm::translate(model, translate);
    model = glm::rotate(model, rotate.w, glm::vec3(rotate));
    model = glm::scale(model, scale);
    glm::mat4 view = (type & 1 ? glm::mat4(glm::mat3(cam.LookAt())) : cam.LookAt()); //Remove translation of skybox in the scene by taking upper-left 3x3 matrix of the 4x4 transformation matrix
    glm::mat4 projection = glm::perspective(glm::radians(FOV), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 MVP = projection * view * model;
    ShaderChief::SetUniMtx4fv("model", glm::value_ptr(model), 0); //Local coords in local/obj space => world coords in world space //SRT
    ShaderChief::SetUniMtx4fv("view", glm::value_ptr(view), 0); //World coords in world space => view coords in view/cam/eye space
    ShaderChief::SetUniMtx4fv("projection", glm::value_ptr(projection), 0); //View coords in view/cam/eye space => clip coords in clip space //aspect ratio?? //++ long dist from near plane to...
    ShaderChief::SetUniMtx4fv("MVP", glm::value_ptr(MVP), 0);
    ShaderChief::SetUni3f("camPos", cam.GetPos().x, cam.GetPos().y, cam.GetPos().z, 0);

    if(~type & 1 && type){
        for(short i = 0; i < 4; ++i){ ///Make Abstract??? //Point light can cover a dist of 50 (from given table)
            ShaderChief::SetUni3f(("pLights[" + std::to_string(i) + "].pos").c_str(), 100.f, 100.f, 100.f); //??
            ShaderChief::SetUni1f(("pLights[" + std::to_string(i) + "].constant").c_str(), 1.f);
            ShaderChief::SetUni1f(("pLights[" + std::to_string(i) + "].linear").c_str(), .09f);
            ShaderChief::SetUni1f(("pLights[" + std::to_string(i) + "].quadratic").c_str(), .032f);
        }
        ShaderChief::SetUni3f("dLight.dir", 0.f, 1.0f, 0.f); //??
        ShaderChief::SetUni3f("sLight.pos", cam.GetPos().x, cam.GetPos().y, cam.GetPos().z);
        ShaderChief::SetUni3f("sLight.dir", cam.CalcFront().x, cam.CalcFront().y, cam.CalcFront().z); //Inefficient??
        ShaderChief::SetUni1f("sLight.cosInnerCutoff", glm::cos(glm::radians(12.5f))); //More efficient than passing in cut-off angle as can directly compare...
        ShaderChief::SetUni1f("sLight.cosOuterCutoff", glm::cos(glm::radians(17.5f))); //...
        ShaderChief::SetUni1f("material.shininess", 32.f); //More light scattering if lower
    }
}

//glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f); //Ortho projection matrix produces clip coords that are NDC alr while perspective projection matrix produces clip coords with a range of -w to w
//Clipped vertices (not in clipping range/vol) are discarded
//Perspective division (dividing gl_Position's xyz coords by its w component) is done automatically after the vertex shader step

//float angle = (float)i / (float)amt * 360.0f;
//float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset; //Multi??
//SetUnis(cam, 2, glm::vec3(sin(angle) * radius + displacement, displacement * 0.4f, cos(angle) * radius + displacement),
//glm::vec3((rand() % 21) / 100.0f + 0.05f), glm::vec4(0.4f, 0.6f, 0.8f, float(rand() % 360))); //transform x and z along the circle and randomly displace along circle with 'radius' in range [-offset, offset]??