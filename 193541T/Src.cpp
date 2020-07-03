#define STB_IMAGE_IMPLEMENTATION
#include "Src.h"
#include "Cam.h"
#include "Model.h"
#include "App.h"

std::vector<Vertex> LoadQuadVertices(){ //Actual winding order is calculated at the rasterization stage after the vertex shader has run //Vertices are then seen from the viewer's POV
    std::vector<Vertex> vertices;
    vertices.emplace_back(Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    vertices.emplace_back(Vertex(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    return vertices;
}

std::vector<Vertex> LoadCubeVertices(){ //In local space (local to mesh)
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

void setMVP(const Cam& cam, const bool& setModel = 1, const glm::vec3& translate = {0.f, 0.f, 0.f}, const glm::vec3& scale = {1.f, 1.f, 1.f}, const glm::vec4& rotate = {0.f, 1.f, 0.f, 0.f}){
    glm::mat4 model = glm::mat4(1.f); //Identity matrix
    model = glm::translate(model, translate);
    model = glm::rotate(model, rotate.w, glm::vec3(rotate));
    model = glm::scale(model, scale);
    glm::mat4 view = cam.LookAt();
    glm::mat4 projection = glm::perspective(glm::radians(App::FOV), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 MVP = projection * view * model;
    if(setModel){
        ShaderChief::SetUniMtx4fv("model", glm::value_ptr(model));
    }
    ShaderChief::SetUniMtx4fv("MVP", glm::value_ptr(MVP)); 

    //Model matrix: local coords in local/obj space => world coords in world space //SRT
    //View matrix: world coords in world space => view coords in view/cam/eye space
    //Projeciton matrix: view coords in view/cam/eye space => clip coords in clip space //aspect ratio?? //++ long dist from near plane to...
    //glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
    //Ortho projection matrix produces clip coords that are NDC alr while perspective projection matrix produces clip coords with a range of -w to w
    //Clipped vertices (not in clipping range/vol) are discarded
    //Perspective division (dividing gl_Position's xyz coords by its w component) is done automatically after the vertex shader step
}

void setUniforms(const Cam& cam, glm::vec3 pos[]){
    setMVP(cam);
    for(short i = 0; i < 4; ++i){ ///Make Abstract??? //Point light can cover a dist of 50 (from given table)
        ShaderChief::SetUni3f(("pLights[" + std::to_string(i) + "].pos").c_str(), pos[i].x, pos[i].y, pos[i].z);
        ShaderChief::SetUni1f(("pLights[" + std::to_string(i) + "].constant").c_str(), 1.f);
        ShaderChief::SetUni1f(("pLights[" + std::to_string(i) + "].linear").c_str(), .09f);
        ShaderChief::SetUni1f(("pLights[" + std::to_string(i) + "].quadratic").c_str(), .032f);
    }
    ShaderChief::SetUni3f("dLight.dir", -0.2f, -1.0f, -0.3f); //vec3 parameter??
    ShaderChief::SetUni3f("sLight.pos", cam.pos.x, cam.pos.y, cam.pos.z);
    ShaderChief::SetUni3f("sLight.dir", cam.CalcFront().x, cam.CalcFront().y, cam.CalcFront().z); //Inefficient??
    ShaderChief::SetUni1f("sLight.cosInnerCutoff", glm::cos(glm::radians(12.5f))); //More efficient than passing in cut-off angle as can directly compare...
    ShaderChief::SetUni1f("sLight.cosOuterCutoff", glm::cos(glm::radians(17.5f))); //...
    ShaderChief::SetUni3f("viewPos", cam.pos.x, cam.pos.y, cam.pos.z); //camPos??
    ShaderChief::SetUni1f("material.shininess", 32.f); //More light scattering if lower
}

uint CreateFramebuffer(uint& texColorBuffer){ //Create framebuffer to get an extra target to render to
    uint FBO;
    glGenFramebuffers(1, &FBO); //Framebuffer is a combination of render buffers stored in GPU mem
    glBindFramebuffer(GL_FRAMEBUFFER, FBO); { //GL_READ_FRAMEBUFFER (read target) is for operations like glReadPixels and GL_DRAW_FRAMEBUFFER (write target)
        /*The main diffs here is that we set the dimensions equal to the screen size(although this is not required) and we pass NULL as the texture's data parameter.
        For this texture, we're only allocating memory and not actually filling it.
        Filling the texture will happen as soon as we render to the framebuffer.
        Also note that we do not care about any of the wrapping methods or mipmapping since we won't be needing those in most cases.*/

        ///An attachment is a mem location that can act as a render buffer for the framebuffer
        glGenTextures(1, &texColorBuffer);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer); { //All rendering commands write to the tex (render output stored inside is used in the shaders)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); //Set the tex's dimensions to win width and height and keep its data uninitialised
            //Call glViewport again before rendering to your framebuffer if render the screen to tex of smaller or larger size??
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0); //Attach colour buffer tex obj to currently bound framebuffer obj //++param info?? //++more colour attachments??

        ///Attach a depth buffer and a stencil buffer as a single tex (each 32-bit value of the tex contains 24 bits of depth info and 8 bits of stencil info)
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL); //Config the tex's formats to contain combined depth and stencil values
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texColorBuffer, 0);

        //Renderbuffer objs store render data directly in their buffer (an arr of stuff) without conversions to texture-specific formats, making them faster as a writeable storage medium
        //Possible to read from renderbuffer objs via the slow glReadPixels which returns a specified area of pixels from the currently bound framebuffer, but not directly from the renderbuffer obj attachments themselves
        //Renderbuffer objs often used as depth and stencil attachments as no need to sample data values in depth and stencil buffer for depth and stencil testing respectively
        //Renderbuffer obj is used only as a framebuffer attachment while tex is a general purpose data buffer
        //Renderbuffer obj's data is in a native format so they are fast when writing data or copying data to other buffers and with operations like switching buffers. The glfwSwapBuffers function may as well be implemented with renderbuffer objects: we simply write to a renderbuffer image, and swap to the other one at the end??
        uint RBO;
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO); {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600); //Create a depth and stencil attachment renderbuffer obj //GL_DEPTH24_STENCIL8 is the internal format (determines precision) which holds a depth buffer with 24 bits and...
        } glBindRenderbuffer(GL_RENDERBUFFER, 0); //Unbind RBO after allocating enuf mem for it
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); //Attach renderbuffer obj to the depth and stencil attachment of the framebuffer

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){ //Verify currently bound framebuffer //++more possibilities??
            printf("Framebuffer is not complete.\n");
        } //All subsequent rendering operations will now render to attachments of the currently bound framebuffer
    } glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return FBO;
}

uint LoadCubemap(std::vector<cstr> faces){
    uint cubeMap;
    glGenTextures(1, &cubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

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

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    //TexCoords exactly between 2 adj faces of the cubemap might not hit an exact face due to hardware limitations so use GL_CLAMP_TO_EDGE to tell OpenGL to return their edge values when tex sampling between faces

    return cubeMap;
}

int main(){
    App* app = new App;
    Cam* cam = new Cam(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f));
    uint texColorBuffer;
    uint FBO = CreateFramebuffer(texColorBuffer);

    ShaderChief* shaderChief = new ShaderChief("Resources/Shaders/Basic.vs", "Resources/Shaders/Basic.fs");
    ShaderChief* lightShaderChief = new ShaderChief("Resources/Shaders/Basic.vs", "Resources/Shaders/Light.fs");
    ShaderChief* outlineShaderChief = new ShaderChief("Resources/Shaders/Basic.vs", "Resources/Shaders/Outline.fs");
    ShaderChief* quadShaderChief = new ShaderChief("Resources/Shaders/Basic.vs", "Resources/Shaders/Quad.fs");
    ShaderChief* screenQuadShaderChief = new ShaderChief("Resources/Shaders/ScreenQuad.vs", "Resources/Shaders/ScreenQuad.fs");
    ShaderChief* cubemapShaderChief = new ShaderChief("Resources/Shaders/Cubemap.vs", "Resources/Shaders/Cubemap.fs");
    ShaderChief* environmentShaderChief = new ShaderChief("Resources/Shaders/Basic.vs", "Resources/Shaders/Environment.fs"); //For environment mapping
    //Model* model = new Model("Resources/Models/Aloe_plant_SF.obj");
    Model* model2 = new Model("Resources/Models/Suit.obj");
    Mesh* mesh = new Mesh(LoadQuadVertices(), {0, 1, 2, 0, 3, 1});
    Mesh* mesh2 = new Mesh(LoadCubeVertices(), {0});
    mesh->LoadTexture("Resources/Textures/blending_transparent_window.png", "d"); //Issues when too close to each other??

    glm::vec3 pointPos[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)
    };
    glm::vec3 quadPos[] = {
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
    uint cubemapRefID = LoadCubemap(texFaces);

    while(!glfwWindowShouldClose(App::win)){ //Render loop
        ///First pass
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //Clear info from prev frame stored in each render buffer with the appropriate bits set //more?? //State-using function
        glEnable(GL_DEPTH_TEST); //Done in screen space after the fragment shader has run and after the stencil test //(pass ? fragment is rendered and depth buffer is updated with new depth value : fragment is discarded)
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); //Set when and how to update the stencil buffer //Default does not write to the stencil buffer //++params and options??
        //GL_REPLACE ensures containers' fragments update the stencil buffer with a stencil value of 1. Because the fragments always pass the stencil test, the stencil buffer is updated with the reference value wherever we've drawn them.
        //++Both outlines for stencil testing

        //Set bitmask that is ANDed with the stencil value abt to be written to the stencil buffer
        glStencilMask(0x00); //Each bit ends up as 0 in the stencil buffer (disabling writing to the stencil buffer)
        //shaderChief->UseProg();
        //setUniforms(*cam, pointPos);
        //model->Draw();
        //model2->Draw();
        //for(short i = 0; i < 4; ++i){
        //    setMVP(*cam, 1, {pointPos[i].x, pointPos[i].y, pointPos[i].z});
        //    mesh2->Draw(0, 1);
        //}

        environmentShaderChief->UseProg();
        ShaderChief::SetUni3f("camPos", cam->pos.x, cam->pos.y, cam->pos.z);
        setMVP(*cam, 1);
        glActiveTexture(GL_TEXTURE30); //??
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapRefID); {
            ShaderChief::SetUni1i("skybox", 30);
            model2->Draw2();
            //mesh2->Draw(0, 0);
        } glBindTexture(GL_TEXTURE_2D, 0);

        ///Update stencil buffer to 1 for each fragment drawn
        glStencilFunc(GL_ALWAYS, 1, 0xFF); //All fragments should pass the stencil test
        glStencilMask(0xFF); //Each bit is written to the stencil buffer unchanged (bitmask of all 1s [default])
        //lightShaderChief->UseProg();
        //for(short i = 0; i < 4; ++i){
        //    setMVP(*cam, 0, {pointPos[i].x, pointPos[i].y, pointPos[i].z});
        //    mesh->Draw(0, 0);
        //}

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF); //So only draw parts of the containers that are not equal to 1 (parts outside the previously drawn containers)
        glStencilMask(0x00);
        //glDisable(GL_DEPTH_TEST); //So up-scaled containers do not get overwritten by the floor??
        //outlineShaderChief->UseProg();
        //float sinTime = (float)sin(glfwGetTime());
        //ShaderChief::SetUni3f("myRGB", sinTime * 2.1f, sinTime * .7f, sinTime * 1.3f);
        //for(short i = 0; i < 4; ++i){
        //    setMVP(*cam, 0, {pointPos[i].x, pointPos[i].y, pointPos[i].z}, glm::vec3(1.25f));
        //    mesh->Draw(0, 0);
        //}

        ///Fragment shader runs for each pixel if skybox renders 1st although only some parts of it eventually becomes visible //Use early depth test to save bandwidth
        ///Render skybox last so depth buffer is filled with depth values of opaque objs and hence call fragment shader to render only fragments of the skybox which pass the early depth test
        glDepthFunc(GL_LEQUAL); //Modify comparison operators used for depth test //Depth buffer filled with 1.0s for the skybox so must ensure the skybox passes the depth tests with depth values <= that in the depth buffer??
        glFrontFace(GL_CW);
        //glDepthMask(GL_FALSE); //Perform depth test on all fragments but not update the depth buffer (use read-only depth buffer) //Need if skybox drawn as 1st obj as it's only 1x1x1
        //Skybox is likely to fail most depth tests and hence fail to render as it's 1x1x1 but cannot just disable depth test as skybox will overwrite all opaque objs
        cubemapShaderChief->UseProg();
        glm::mat4 view = glm::mat4(glm::mat3(cam->LookAt())); //Remove translation by taking the upper-left 3x3 matrix of the 4x4 transformation matrix??
        glm::mat4 projection = glm::perspective(glm::radians(App::FOV), 800.0f / 600.0f, 0.1f, 100.0f);
        ShaderChief::SetUniMtx4fv("view", glm::value_ptr(view));
        ShaderChief::SetUniMtx4fv("projection", glm::value_ptr(projection));
        glActiveTexture(GL_TEXTURE30);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapRefID); {
            ShaderChief::SetUni1i("cubemap", 30);
            mesh2->Draw(0, 0);
        } glBindTexture(GL_TEXTURE_2D, 0);
        //glDepthMask(GL_TRUE);
        glFrontFace(GL_CCW); //Default
        glDepthFunc(GL_LESS); //Default

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        //glEnable(GL_DEPTH_TEST);

        ///Draw opaque objs first so dst colour used in blend eqn is correct
        //std::map<float, glm::vec3> sorted; //DS from STL lib which sorts its values based on its keys
        //for(uint i = 0; i < sizeof(quadPos) / sizeof(quadPos[0]); ++i){ //Transparent parts are written to the depth buffer like any other value so they are depth-tested like any other opaque obj would be (depth test discards other parts behind transparent parts)
        //    float dist = glm::length(cam->pos - quadPos[i]);
        //    sorted[dist] = quadPos[i];
        //} //Does not take rotations and scaling into acct and weirdly shaped objs need a diff metric than simply a pos vector
        ////Can choose discarding fragments over blending them for fully transparent objs so no depth issues
        //quadShaderChief->UseProg();
        //ShaderChief::SetUni1i("texture1", 0);
        //for(std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it){
        //    setMVP(*cam, 0, it->second);
        //    mesh->Draw(1, 0);
        //}

        ///Second pass1
        glBindFramebuffer(GL_FRAMEBUFFER, 0); //Stop off-screen rendering by making the default framebuffer active //Unbind the framebuffer so won't accidentally render to the wrong framebuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //more/less??

        app->Update();
        cam->Update(GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S);

        screenQuadShaderChief->UseProg();
        glDisable(GL_DEPTH_TEST); //So screen quad renders in front of everything else
        glActiveTexture(GL_TEXTURE31);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer); {
            ShaderChief::SetUni1i("screenTexture", 31); //Weird output without this only??
            mesh->Draw(1, 0);
        } glBindTexture(GL_TEXTURE_2D, 0);

        glfwSwapBuffers(App::win); //Swap the large 2D colour buffer containing colour values for each pixel in GLFW's window
        glfwPollEvents(); //Check for triggered events and call corresponding functions registered via callback methods
    }
    delete app;
    delete cam;
    delete shaderChief;
    delete lightShaderChief;
    delete outlineShaderChief;
    delete mesh;
    glDeleteFramebuffers(1, &FBO);
	_CrtDumpMemoryLeaks();
}