#include "App.h"

bool App::firstCall = 1;
float App::dt = 0.f;
float App::FOV = 45.f;
float App::pitch = 0.f;
float App::yaw = -90.f;
float App::lastX = 0.f;
float App::lastY = 0.f;
float App::sensitivity = .05f;
GLFWwindow* App::win = 0;

App::App(){
    Init();
}

App::~App(){
    glfwTerminate(); //Clean/Del all GLFW's resources that were allocated
}

void FramebufferSizeCallback(GLFWwindow*, int width, int height){ //Resize callback
    glViewport(0, 0, width, height); //For viewport transform
}

void ScrollCallback(GLFWwindow*, double xOffset, double yOffset){
    App::FOV -= float(xOffset) + float(yOffset);
    App::FOV = std::max(1.f, std::min(75.f, App::FOV));
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

void App::Init(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //For Mac OS X

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
    glEnable(GL_STENCIL_TEST); //Make all render calls influence the stencil buffer //Discard fragments based on fragments of other drawn objs in the scene
    glStencilFunc(GL_EQUAL, 1, 0xFF); //The frag passes... and is drawn if its stencil value is equal to 1 (ref value) //++params??

    glEnable(GL_BLEND); //Colour resulting from blend eqn replaces prev colour stored in the colour buffer
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //++options??
    //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO); //sets the RGB components as we've set them previously, but only lets the resulting alpha component be influenced by the source's alpha value??
    //glBlendEquation(GL_FUNC_ADD); //Change operator between src and dst part of blend eqn //++other blend eqns??

    glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT_AND_BACK);
}

void App::Update(){
    //Colour buffer (stores all the fragment colours: the visual output)
    //Z-buffer/Depth buffer (stores depth value of each fragment as 16, 24 or 32 bit floats, has same width and height as the colour buffer)
    glfwSetWindowShouldClose(win, glfwGetKey(win, GLFW_KEY_ESCAPE));
    ///++bounceTime??
    GLint polyMode;
    glGetIntegerv(GL_POLYGON_MODE, &polyMode);
    if(glfwGetKey(win, GLFW_KEY_2)){
        glPolygonMode(GL_FRONT_AND_BACK, polyMode + (polyMode == GL_FILL ? -2 : 1));
    }

    float currFrame = (float)glfwGetTime();
    dt = currFrame - lastFrame;
    lastFrame = currFrame;
}