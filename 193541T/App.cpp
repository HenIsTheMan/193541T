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

App::App(): cullBT(0.f), polyModeBT(0.f){
    Init();
    scene = new Scene;
}

App::~App(){
    delete scene;
    glDeleteFramebuffers(1, &frontFBO);
    glDeleteFramebuffers(1, &backFBO);
    glDeleteFramebuffers(1, &enFBO);
    glDeleteFramebuffers(1, &intermediateFBO);
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

void App::CreateFramebuffer(uint& FBO, uint& texColourBuffer){ //Create framebuffer (combination of render buffers stored in GPU mem) to get an extra target to render to)
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO); {
        glGenTextures(1, &texColourBuffer);
        glBindTexture(GL_TEXTURE_2D, texColourBuffer); {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColourBuffer, 0); //Only need colour buffer

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
            printf("Created framebuffer is not complete.\n");
        }
    } glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void App::CreateMultisampleFramebuffer(uint& FBO){ //MSAA uses a much larger depth or stencil buffer to determine subsample coverage after the fragment shader is run once per pixel for each primitive with vertex data interpolated to the center of each pixel //Amt of subsamples covered affects how much pixel colour mixes with curr framebuffer colour (latest clear colour if 1st draw/...)
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO); { //GL_READ_FRAMEBUFFER (read target) is for operations like glReadPixels and GL_DRAW_FRAMEBUFFER (write target)
        uint texColourBuffer; //Colour buffer (stores all the fragment colours: the visual output)
        glGenTextures(1, &texColourBuffer);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texColourBuffer); { //All rendering commands write to the tex (render output stored inside is used in the shaders)
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); //Set tex's dimensions to screen size (not required) and NULL to allocate mem (data uninitialised) //Render to framebuffer to fill tex //Call glViewport again before rendering to your framebuffer if render the screen to tex of smaller or larger size??
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, 800, 600, GL_TRUE); //Multisampled tex attachment //If the last argument is set to GL_TRUE, the image will use identical sample locations and the same number of subsamples for each texel??
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texColourBuffer, 0); //Attach colour buffer tex obj to currently bound framebuffer obj //++param info?? //++more colour attachments??
        //Colour values are stored once per pixel so colour buffer size unaffected by...

        ///Attach a z-buffer/depth buffer (stores depth value of each fragment as 16, 24 or 32 bit floats, same width and height as colour buffer) and a stencil buffer as a single tex (each 32-bit value of the tex contains 24 bits of depth info and 8 bits of stencil info)
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL); //Config the tex's formats to contain combined depth and stencil values
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texColorBuffer, 0);

        uint RBO; //RBOs store render data directly (so data in native format) in their buffer (an arr of stuff) without conversions to texture-specific formats so fast as a writeable storage medium (fast when writing or copying data to other buffers and with operations like switching buffers) //The glfwSwapBuffers function may as well be implemented with RBOs (simply write to a renderbuffer img, and swap to the other one at the end)??
        glGenRenderbuffers(1, &RBO); //Can read from RBOs via the slow glReadPixels which returns a specified area of pixels from the currently bound framebuffer but not directly from the RBO attachments themselves
        glBindRenderbuffer(GL_RENDERBUFFER, RBO); { //RBOs (only as a framebuffer attachment [mem location that can act as a render buffer for the framebuffer] while texs are general purpose data buffers) often used as depth and stencil attachments as no need to sample data values in depth and stencil buffer for depth and stencil testing respectively
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, 800, 600); //Config RBO's mem storage //Create a depth and stencil attachment renderbuffer obj //GL_DEPTH24_STENCIL8 is the internal format (determines precision) which holds a depth buffer with 24 bits and...
        } glBindRenderbuffer(GL_RENDERBUFFER, 0); //Unbind RBO after allocating enuf mem for it
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); //Attach renderbuffer obj to the depth and stencil attachment of the framebuffer
        //A vertex's depth value is interpolated to each subsample before depth testing while stencil values are stored per subsample before... so size of depth and stencil buffers rises by qty of subsamples per pixel

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){ //Verify currently bound framebuffer //++more possibilities??
            printf("Created multisample framebuffer is not complete.\n");
        }
    } glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void App::Init(){
    glfwInit();
    //glfwWindowHint(GLFW_SAMPLES, 4); //4 subsamples in a general pattern per set of screen coords of a pixel to determine pixel coverage //Better pixel coverage precision but more performance reduction with more sample pts as they cause size of... buffers to rise by...
    //Super Sample Anti-Aliasing (SSAA, draw more fragments, sample pt in the center of each pixel determines if each pixel is influenced by any fragment shader or not) temporarily uses a much higher resolution render buffer to render to and the resolution is downsampled back to normal after the scene is rendered
    //Result of Multisample Anti-Aliasing (MSAA) is a framebuffer with higher resolution depth or stencil buffer where primitive edges are smoother
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

    CreateMultisampleFramebuffer(frontFBO);
    CreateMultisampleFramebuffer(backFBO);
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
    CreateMultisampleFramebuffer(enFBO);
    CreateFramebuffer(intermediateFBO, intermediateTex);

    glEnable(GL_PROGRAM_POINT_SIZE);
    //glEnable(GL_MULTISAMPLE); //Enabled by default on most OpenGL drivers //Multisample buffer (stores a given amt of sample pts per pixel) for MSAA //Multisampling algs are implemented in the rasterizer (combination of all algs and processes that transform vertices of primitives into fragments, fragments are bound by screen resolution unlike vertices so there is almost nvr a 1-on-1 mapping between... and it must decide at what screen coords will each fragment of each interpolated vertex end up at) of the OpenGL drivers
    //Poor screen resolution leads to aliasing as the limited amt of screen pixels causes some pixels to not be rendered along an edge of a fig //Colour output is stored directly in the framebuffer if pixel is fully covered and blending is disabled
}

void App::Update(){
    float currFrame = (float)glfwGetTime();
    dt = currFrame - lastFrame;
    lastFrame = currFrame;
    glfwSetWindowShouldClose(win, glfwGetKey(win, GLFW_KEY_ESCAPE));

    if(glfwGetKey(win, GLFW_KEY_1) && cullBT <= currFrame){
        glIsEnabled(GL_CULL_FACE) ? glDisable(GL_CULL_FACE) : glEnable(GL_CULL_FACE);
        cullBT = currFrame + .5f;
    }

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
        glBindFramebuffer(GL_FRAMEBUFFER, enFBO); //All subsequent rendering operations will now render to attachments of the currently bound framebuffer
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //State-using function
    scene->RenderToCreatedFB(backCam, &enTex);

    //glBindFramebuffer(GL_READ_FRAMEBUFFER, frontFBO);
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    //glBlitFramebuffer(0, 0, 800, 600, 0, 0, 800, 600, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, backFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
    glBlitFramebuffer(0, 0, 800, 600, 0, 0, 800, 600, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); //Stop off-screen rendering by making the default framebuffer active //Unbind the framebuffer so won't accidentally render to the wrong framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    scene->RenderToDefaultFB(intermediateTex, glm::vec3(-.5f, .5f, 0.f), glm::vec3(.5f));

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, frontFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
    glBlitFramebuffer(0, 0, 800, 600, 0, 0, 800, 600, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); //Stop off-screen rendering by making the default framebuffer active //Unbind the framebuffer so won't accidentally render to the wrong framebuffer
    scene->RenderToDefaultFB(intermediateTex); //Some post-processing filters like edge detection will produce jagged edges as intermediateTex is a non-multisampled tex (do post-processing later or use anti-aliasing alg to correct)
    glStencilFunc(GL_ALWAYS, 1, 0xFF);

    //glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    //scene->RenderToDefaultFB(frontTexColourBuffer);
    //glStencilFunc(GL_ALWAYS, 1, 0xFF);
}