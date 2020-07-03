#define STB_IMAGE_IMPLEMENTATION
#include "Src.h"
#include "ShaderChief.h"
#include "stb_image.h"
#include <algorithm>
#include <numeric>
#include "Cam.h"

float FOV = 45.0f;
float lastX, lastY;
float sensitivity = 0.05f;

void FramebufferSizeCallback(GLFWwindow*, int width, int height){ //Resize callback
    glViewport(0, 0, width, height); //For viewport transform
}

void ScrollCallback(GLFWwindow*, double xOffset, double yOffset){
    FOV -= float(xOffset) + float(yOffset);
    FOV = std::max(1.0f, std::min(75.f, FOV));
}

bool firstCall = 1;
float pitch = 0.f, yaw = -90.0f; //Euler angles //++roll
void CursorPosCallback(GLFWwindow*, double xPos, double yPos){
    ///Add mouse movement offset between last frame and curr frame
    if(firstCall){
        firstCall = 0;
    } else{
        yaw += (float(xPos) - lastX) * sensitivity;
        pitch += (lastY - float(yPos)) * sensitivity; //Reversed as y-coords range from bottom to top
        pitch = std::max(-89.0f, std::min(89.0f, pitch)); //Prevent LookAt flip once front vec // worldUp vec
    }
    lastX = float(xPos);
    lastY = float(yPos);
}

float proportion = 0.f;
float deltaTime = 0.0f;	//Time between curr frame and last frame
float lastFrame = 0.0f; //Time of last frame

//bool Key(unsigned short key){ //??
//    #include <winuser.h>
//    return (GetAsyncKeyState(key) & 0x8001) != 0;
//}

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //For Mac OS X

    GLFWwindow* win = glfwCreateWindow(800, 600, "193541T", 0, 0); //Get a handle to the created window obj
    if(win == 0){
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(win); //Make context of the window the main context on the curr thread

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        printf("Failed to init GLAD\n");
        return -1;
    }
    glfwSetFramebufferSizeCallback(win, &FramebufferSizeCallback);

    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //Hide and capture mouse cursor
    glfwSetCursorPosCallback(win, CursorPosCallback);
    glfwSetScrollCallback(win, ScrollCallback);
    stbi_set_flip_vertically_on_load(1); //Ensure textures are not flipped upside-down

    float vertices[] = { //In local space (local to mesh)
        //Positions          //Colors          //Texture   //Normals
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    ///Textures
    int width, height, colourChannelsAmt;
    cstr filePaths[2]{"Resources/Imgs/container.jpg", "Resources/Imgs/awesomeface.png"};
    uint tex[2];
    for(short i = 0; i < 2; ++i){
        unsigned char* data = stbi_load(filePaths[i], &width, &height, &colourChannelsAmt, 0);
        glGenTextures(1, &tex[i]);
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, tex[i]); //Make 'tex' the texture currently bound to the currently active texture unit so subsequent texture commands will configure it

        ///For Texture Wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); //GL_TEXTURE_2D is the texture target
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT); //For 3D textures, texture axes are S, T and R
        float borderColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        ///For Texture Filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //Nearest neighbour/Point filtering/interpolation when textures are scaled downwards
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //Linear filtering/interpolation for upscaled textures
        //++more??
        //++ Mipmaps for downscaled textures

        if(data[i]){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB + i, GL_UNSIGNED_BYTE, data); //GL_RGBA for both??
            glGenerateMipmap(GL_TEXTURE_2D); //Gen required mipmap lvls for the currently bound texture
        } else{
            printf("Failed to load texture.\n");
        }
        stbi_image_free(data); //Free the img mem
    }

    Cam cam(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    ShaderChief shaderMan("Resources/Shaders/Basic.vs", "Resources/Shaders/Basic.fs");

    ///Make sure each shader sampler uniform corresponds to the correct texture unit
    shaderMan.SetUni1i("texture1", 0);
    shaderMan.SetUni1i("texture2", 1);

    ShaderChief lightShaderMan("Resources/Shaders/Light.vs", "Resources/Shaders/Light.fs");
    glm::mat4 model, view, projection;
    glClearColor(0.2f, 0.3f, 0.3f, 1.f); //State-setting function
    glEnable(GL_DEPTH_TEST);

    while(!glfwWindowShouldClose(win)){ //Render loop
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear info stored in buffers from prev frame if any //diff types?? //State-using function

        glm::vec3 lightPos((float)cos(glfwGetTime()) * (float)cos(glfwGetTime()), (float)sin(glfwGetTime()),
            (float)sin(glfwGetTime()) * (float)cos(glfwGetTime()));

        glfwSetWindowShouldClose(win, glfwGetKey(win, GLFW_KEY_ESCAPE));
        ///++bounceTime??
        GLint polyMode;
        glGetIntegerv(GL_POLYGON_MODE, &polyMode);
        if(glfwGetKey(win, GLFW_KEY_2)){
            glPolygonMode(GL_FRONT_AND_BACK, polyMode + (polyMode == GL_FILL ? -2 : 1));
        }
        proportion += (float)(glfwGetKey(win, GLFW_KEY_0) - glfwGetKey(win, GLFW_KEY_9)) / 10.f;
        proportion = std::max(0.f, std::min(1.f, proportion));
        shaderMan.SetUni1f("proportion", proportion);

        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        cam.SetSpd(2.5f * deltaTime);
        cam.inputs.upDown = float(glfwGetKey(win, GLFW_KEY_Q) - glfwGetKey(win, GLFW_KEY_E));
        cam.inputs.leftRight = float(glfwGetKey(win, GLFW_KEY_A) - glfwGetKey(win, GLFW_KEY_D));
        cam.inputs.frontBack = float(glfwGetKey(win, GLFW_KEY_W) - glfwGetKey(win, GLFW_KEY_S));
        cam.Update(pitch, yaw);

        shaderMan.UseProg();
        uint VAO, VBO, EBO; //Store ref ID
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO); //Gen VBO and get ref ID of it
        glBindBuffer(GL_ARRAY_BUFFER, VBO); //Makes VBO the buffer currently bound to the GL_ARRAY_BUFFER target, GL_ARRAY_BUFFER is VBO's type
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Copies vertex data stored in 'vertices' into VBO's mem //diff types??
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //GL_ELEMENT_ARRAY_BUFFER is the buffer target
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        ///Attribs: pos, colour, texture
        int componentsAmt[]{3, 3, 2, 3}, arrSize = sizeof(componentsAmt) / sizeof(componentsAmt[0]);
        for(int i = 0; i < arrSize; ++i){
            size_t floatSize = sizeof(float);
            glVertexAttribPointer(i, componentsAmt[i], GL_FLOAT, GL_FALSE, GLsizei(std::accumulate(componentsAmt, componentsAmt + arrSize, 0) * floatSize),
                (void*)((!i ? 0 : std::accumulate(componentsAmt, componentsAmt + i, 0)) * floatSize));
            glEnableVertexAttribArray(i);
        }

        for(uint i = 0; i < 10; ++i){
            float angle = 20.0f * (i + 1) * !(i % 3);
            model = glm::rotate(glm::translate(glm::mat4(1.0f), cubePositions[i]),
                (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f)); //Local coords in local/obj space => world coords in world space //SRT
            shaderMan.SetUniMtx4fv("model", glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36); //GL_TRIANGLES??
            //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //Render from index buffer rather than vertex buffer
        }

        view = cam.LookAt(); //World coords in world space => view coords in view/cam/eye space
        projection = glm::perspective(glm::radians(FOV), 800.0f / 600.0f, 0.1f, 100.0f); //View coords in view/cam/eye space => clip coords in clip space //aspect ratio?? //++ long dist from near plane to...
        //glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
        //Ortho projection matrix produces clip coords that are NDC alr while perspective projection matrix produces clip coords with a range of -w to w
        //Clipped vertices (not in clipping range/vol) are discarded
        //Perspective division is done automatically after the vertex shader step

        shaderMan.SetUniMtx4fv("view", glm::value_ptr(view));
        shaderMan.SetUniMtx4fv("projection", glm::value_ptr(projection));

        ///Another way??
        shaderMan.SetUni3f("material.ambient", 1.0f, 0.5f, 0.31f);
        shaderMan.SetUni3f("material.diffuse", 1.0f, 0.5f, 0.31f);
        shaderMan.SetUni3f("material.specular", 0.5f, 0.5f, 0.5f);
        shaderMan.SetUni1f("material.shininess", 32.0f);

        glm::vec3 lightColor;
        lightColor.x = float(sin(glfwGetTime() * 2.0f)); //rand()??
        lightColor.y = float(sin(glfwGetTime() * 0.7f));
        lightColor.z = float(sin(glfwGetTime() * 1.3f));

        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

        shaderMan.SetUni3f("light.ambient", ambientColor.x, ambientColor.y, ambientColor.z); //vec3 parameter??
        shaderMan.SetUni3f("light.diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z); //Darken diffuse component of light a little bit
        shaderMan.SetUni3f("light.specular", 1.0f, 1.0f, 1.0f);
        shaderMan.SetUni3f("light.pos", lightPos.x, lightPos.y, lightPos.z);

        shaderMan.SetUni3f("viewPos", cam.pos.x, cam.pos.y, cam.pos.z); //camPos??
        glBindVertexArray(0); //Break the existing vertex arr obj binding

        lightShaderMan.UseProg();
        uint lightVAO;
        glGenVertexArrays(1, &lightVAO);
        glBindVertexArray(lightVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightShaderMan.SetUniMtx4fv("model", glm::value_ptr(model));
        lightShaderMan.SetUniMtx4fv("view", glm::value_ptr(view));
        lightShaderMan.SetUniMtx4fv("projection", glm::value_ptr(projection));

        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0); //Break the existing vertex arr obj binding

        glfwSwapBuffers(win); //Swap the large 2D colour buffer containing colour values for each pixel in GLFW's window
        glfwPollEvents(); //Check for triggered events and call corresponding functions registered via callback methods
    }
    glfwTerminate(); //Clean/Del all GLFW's resources that were allocated
	_CrtDumpMemoryLeaks();
}