#include "App.h"

bool App::firstCall = 1;
float App::dt = 0.f;
float App::pitch = 0.f;
float App::yaw = 0.f;
float App::lastX = 0.f;
float App::lastY = 0.f;
float App::sensitivity = .05f;
GLFWwindow* App::win = 0; //Render window

float FOV = 45.f;

App::App(): polyModeBT(0.f){
    Init();
    scene = new Scene;
}

App::~App(){
    delete scene;
    glDeleteFramebuffers(1, &frontFBO);
    glDeleteFramebuffers(1, &backFBO);
    glfwTerminate(); //Clean/Del all GLFW's resources that were allocated
}

void FramebufferSizeCallback(GLFWwindow*, int width, int height){ //Resize callback
    glViewport(0, 0, width, height); //For viewport transform
}

void ScrollCallback(GLFWwindow*, double xOffset, double yOffset){
    FOV -= float(xOffset) + float(yOffset);
    FOV = std::max(1.f, std::min(75.f, FOV));
}

void CursorPosCallback(GLFWwindow*, double xPos, double yPos){
    if(App::firstCall){
        App::firstCall = 0;
    } else{ //Add mouse movement offset between last frame and curr frame
        App::yaw += (float(xPos) - App::lastX) * App::sensitivity;
        App::pitch += (App::lastY - float(yPos)) * App::sensitivity; //Reversed as y-coords range from bottom to top
        App::pitch = std::max(-89.0f, std::min(89.0f, App::pitch)); //Prevent LookAt flip once front vec // worldUp vec
    }
    App::lastX = float(xPos);
    App::lastY = float(yPos);
}

bool App::Key(int key){
    return bool(glfwGetKey(win, key));
}

void App::CreateFramebuffer(uint& FBO, uint& texColourBuffer, uint& RBO){ //Create framebuffer to get an extra target to render to
    glGenFramebuffers(1, &FBO); //Framebuffer is a combination of render buffers stored in GPU mem
    glBindFramebuffer(GL_FRAMEBUFFER, FBO); { //GL_READ_FRAMEBUFFER (read target) is for operations like glReadPixels and GL_DRAW_FRAMEBUFFER (write target)
        /*The main diffs here is that we set the dimensions equal to the screen size(although this is not required) and we pass NULL as the texture's data parameter.
        For this texture, we're only allocating memory and not actually filling it.
        Filling the texture will happen as soon as we render to the framebuffer.
        Also note that we do not care about any of the wrapping methods or mipmapping since we won't be needing those in most cases.*/

        ///An attachment is a mem location that can act as a render buffer for the framebuffer
        glGenTextures(1, &texColourBuffer); //Colour buffer (stores all the fragment colours: the visual output)
        glBindTexture(GL_TEXTURE_2D, texColourBuffer); { //All rendering commands write to the tex (render output stored inside is used in the shaders)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); //Set the tex's dimensions to win width and height and keep its data uninitialised
            //Call glViewport again before rendering to your framebuffer if render the screen to tex of smaller or larger size??
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColourBuffer, 0); //Attach colour buffer tex obj to currently bound framebuffer obj //++param info?? //++more colour attachments??

        ///Attach a depth buffer and a stencil buffer as a single tex (each 32-bit value of the tex contains 24 bits of depth info and 8 bits of stencil info)
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL); //Config the tex's formats to contain combined depth and stencil values
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texColorBuffer, 0);

        //Renderbuffer objs store render data directly in their buffer (an arr of stuff) without conversions to texture-specific formats, making them faster as a writeable storage medium
        //Possible to read from renderbuffer objs via the slow glReadPixels which returns a specified area of pixels from the currently bound framebuffer, but not directly from the renderbuffer obj attachments themselves
        //Renderbuffer objs often used as depth and stencil attachments as no need to sample data values in depth and stencil buffer for depth and stencil testing respectively
        //Renderbuffer obj is used only as a framebuffer attachment while tex is a general purpose data buffer
        //Renderbuffer obj's data is in a native format so they are fast when writing data or copying data to other buffers and with operations like switching buffers. The glfwSwapBuffers function may as well be implemented with renderbuffer objects: we simply write to a renderbuffer image, and swap to the other one at the end??
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO); {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600); //Create a depth and stencil attachment renderbuffer obj //GL_DEPTH24_STENCIL8 is the internal format (determines precision) which holds a depth buffer with 24 bits and...
        } glBindRenderbuffer(GL_RENDERBUFFER, 0); //Unbind RBO after allocating enuf mem for it
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); //Attach renderbuffer obj to the depth and stencil attachment of the framebuffer

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){ //Verify currently bound framebuffer //++more possibilities??
            printf("Framebuffer is not complete.\n");
        } //All subsequent rendering operations will now render to attachments of the currently bound framebuffer
    } glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void App::Init(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //For Mac OS X
    #endif

    win = glfwCreateWindow(800, 600, "193541T", 0, 0);
    if(win == 0){ //Get a handle to the created window obj
        printf("Failed to create GLFW window\n");
    }
    glfwMakeContextCurrent(win); //Make context of the window the main context on the curr thread

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        printf("Failed to init GLAD\n");
    }
    glfwSetFramebufferSizeCallback(win, &FramebufferSizeCallback);

    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //Hide and capture mouse cursor
    glfwSetCursorPosCallback(win, CursorPosCallback);
    glfwSetScrollCallback(win, ScrollCallback);
    //stbi_set_flip_vertically_on_load(1); //Ensure textures are not flipped upside-down
    glClearColor(0.2f, 0.3f, 0.3f, 1.f); //State-setting function

    //Stencil buffer usually contains 8 bits per stencil value that amts to 256 diff stencil values per pixel
    //Use stencil buffer operations to write to the stencil buffer when rendering fragments (read stencil values in the same or following frame(s) to pass or discard fragments based on their stencil value)
    glEnable(GL_STENCIL_TEST); //Discard fragments based on fragments of other drawn objs in the scene
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); //Fragments update stencil buffer with their ref value when... //++params and options??

    glEnable(GL_DEPTH_TEST); //Done in screen space after the fragment shader has run and after the stencil test //(pass ? fragment is rendered and depth buffer is updated with new depth value : fragment is discarded)

    glEnable(GL_BLEND); //Colour resulting from blend eqn replaces prev colour stored in the colour buffer
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //++options??
    //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO); //sets the RGB components as we've set them previously, but only lets the resulting alpha component be influenced by the source's alpha value??
    //glBlendEquation(GL_FUNC_ADD); //Change operator between src and dst part of blend eqn //++other blend eqns??

    glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT_AND_BACK);

    CreateFramebuffer(frontFBO, frontTexColourBuffer, frontRBO);
    CreateFramebuffer(backFBO, backTexColourBuffer, backRBO);
    //Z-buffer/Depth buffer (stores depth value of each fragment as 16, 24 or 32 bit floats, same width and height as colour buffer)

    glGenTextures(1, &enTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, enTex); {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        for(short i = 0; i < 6; ++i){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); //GL_RGB8?? //GL_RGB32F??
        }
    } glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    CreateFramebuffer(enFBO, enTexColourBuffer, enRBO);

    glEnable(GL_PROGRAM_POINT_SIZE);
}

void App::Update(){
    float currFrame = (float)glfwGetTime();
    dt = currFrame - lastFrame;
    lastFrame = currFrame;
    glfwSetWindowShouldClose(win, glfwGetKey(win, GLFW_KEY_ESCAPE));
    GLint polyMode;
    glGetIntegerv(GL_POLYGON_MODE, &polyMode);
    if(glfwGetKey(win, GLFW_KEY_2) && polyModeBT <= currFrame){
        glPolygonMode(GL_FRONT_AND_BACK, polyMode + (polyMode == GL_FILL ? -2 : 1));
        polyModeBT = currFrame + .5f;
    }
}

void App::Render(const Cam& cam){
    ///For dynamic environemnt mapping (use dynamically-generated cubemap textured with 6 diff angles of the scene as seen from an instance to create reflective and refractive surfaces that include other instances, avoid and/or use pre-rendered cubemaps as expensive)
    float initialFOV = FOV;
    FOV = 90.f;
    for(short i = 0; i < 6; ++i){
        Cam enCam(glm::vec3(0.f), glm::vec3(0.f));
        switch(i){
            case 0: enCam.SetTarget(glm::vec3(1.f, 0.f, 0.f)); break;
            case 1: enCam.SetTarget(glm::vec3(-1.f, 0.f, 0.f)); break;
            case 2: enCam.SetTarget(glm::vec3(0.f, 1.f, 0.f)); break;
            case 3: enCam.SetTarget(glm::vec3(0.f, -1.f, 0.f)); break;
            case 4: enCam.SetTarget(glm::vec3(0.f, 0.f, 1.f)); break;
            default: enCam.SetTarget(glm::vec3(0.f, 0.f, -1.f));
        }
        glBindFramebuffer(GL_FRAMEBUFFER, enFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, enTex, 0); //Render to enFBO multiple times with a diff tex target each time
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        scene->RenderToCreatedFB(enCam, 0);
    }
    FOV = initialFOV;

    glBindFramebuffer(GL_FRAMEBUFFER, frontFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //Clear info from prev frame stored in each render buffer with the appropriate bits set
    scene->RenderToCreatedFB(cam, &enTex);

    Cam backCam(cam.GetPos(), cam.GetPos() + cam.GetPos() - cam.GetTarget());
    glBindFramebuffer(GL_FRAMEBUFFER, backFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    scene->RenderToCreatedFB(backCam, &enTex);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); //Stop off-screen rendering by making the default framebuffer active //Unbind the framebuffer so won't accidentally render to the wrong framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //State-using function
    scene->RenderToDefaultFB(backTexColourBuffer, glm::vec3(-.5f, .5f, 0.f), glm::vec3(.5f));
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    scene->RenderToDefaultFB(frontTexColourBuffer);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
}