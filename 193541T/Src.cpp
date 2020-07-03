#define STB_IMAGE_IMPLEMENTATION
#include "Src.h"
#include "ShaderChief.h"
#include "stb_image.h"
#include <algorithm>

void framebuffer_size_callback(GLFWwindow*, int width, int height){ //resize callback
    glViewport(0, 0, width, height);
}

float proportion = 0.f;

void processInput(GLFWwindow* win){
    if(glfwGetKey(win, GLFW_KEY_ESCAPE)){
        glfwSetWindowShouldClose(win, 1);
    }
    if(glfwGetKey(win, GLFW_KEY_3)){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if(glfwGetKey(win, GLFW_KEY_4)){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if(glfwGetKey(win, GLFW_KEY_5)){
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }
    proportion += (float)(glfwGetKey(win, GLFW_KEY_0) - glfwGetKey(win, GLFW_KEY_9)) / 10.f; //++bounceTime??
    proportion = std::max(0.f, std::min(1.f, proportion));
}

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
    glfwSetFramebufferSizeCallback(win, &framebuffer_size_callback); //??

    float vertices[] = {
        // positions       // colors       // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f, 2.0f,   // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f,   // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 2.0f    // top left 
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    stbi_set_flip_vertically_on_load(1); //Ensure textures are not flipped upside-down

    ///for loop??
    int width, height, colourChannelsAmt;
    unsigned char* data = stbi_load("Resources/Imgs/container.jpg", &width, &height, &colourChannelsAmt, 0);
    uint tex;
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex); //Make 'tex' the texture currently bound to the currently active texture unit so subsequent texture commands will configure it

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

    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D); //Gen required mipmap lvls for the currently bound texture
    } else{
        printf("Failed to load texture.\n");
    }
    stbi_image_free(data); //Free the img mem


    data = stbi_load("Resources/Imgs/awesomeface.png", &width, &height, &colourChannelsAmt, 0);
    uint tex2;
    glGenTextures(1, &tex2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex2); //Make 'tex' the texture currently bound to the currently active texture unit so subsequent texture commands will configure it

    ///For Texture Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //GL_TEXTURE_2D is the texture target
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT); //For 3D textures, texture axes are S, T and R

    ///For Texture Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //Nearest neighbour/Point filtering/interpolation when textures are scaled downwards
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //Linear filtering/interpolation for upscaled textures
    //++more??
    //++ Mipmaps for downscaled textures

    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //GL_RGBA??
        glGenerateMipmap(GL_TEXTURE_2D); //Gen required mipmap lvls for the currently bound texture
    } else{
        printf("Failed to load texture.\n");
    }
    stbi_image_free(data); //Free the img mem

    //glm::mat4 trans = glm::mat4(1.0f);
    //trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0)); //why??
    //trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

    ShaderChief shaderMan("Resources/Shaders/Basic.vs", "Resources/Shaders/Basic.fs");
    //float colour[4]{0.f, ((float)sin(glfwGetTime()) / 2.f) + .5f /*0.f - 1.f*/, 0.0f, 1.0f};
    //shaderMan.SetUni("ourColor", colour);

    ///Make sure each shader sampler uniform corresponds to the correct texture unit
    shaderMan.SetUni("texture1", 0);
    shaderMan.SetUni("texture2", 1);

    while(!glfwWindowShouldClose(win)){ //Render loop
        processInput(win);
        shaderMan.SetUni("proportion", proportion);

        glClearColor(0.2f, 0.3f, 0.3f, 1.f); //State-setting function
        glClear(GL_COLOR_BUFFER_BIT); //diff types?? //State-using function

        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        unsigned int VBO; //Stores ref ID of VBO //arr?? //GLuint??
        glGenBuffers(1, &VBO); //Gen VBO and get ref ID of it
        glBindBuffer(GL_ARRAY_BUFFER, VBO); //Makes VBO the buffer currently bound to the GL_ARRAY_BUFFER target, GL_ARRAY_BUFFER is VBO's type
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Copies vertex data stored in 'vertices' into VBO's mem //diff types??

        unsigned int EBO;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //GL_ELEMENT_ARRAY_BUFFER is the buffer target
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        //glm::mat4 trans = glm::mat4(1.0f);
        //trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        //trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        //shaderMan.SetUni("transform", glm::value_ptr(trans));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));  // note that we're translating the scene in the reverse direction of where we want to move

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f); //Perspective projection matrix

        shaderMan.SetUni("model", glm::value_ptr(model));
        shaderMan.SetUni("view", glm::value_ptr(view));
        shaderMan.SetUni("projection", glm::value_ptr(projection));

        ///for loop??
        //Pos attrib
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        //Colour attrib
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        //Tex attrib
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        //glDrawArrays(GL_TRIANGLES, 0, 3); //GL_TRIANGLES??
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //Render from index buffer rather than vertex buffer
        glBindVertexArray(0); //Break the existing vertex arr obj binding

        glfwSwapBuffers(win); //Swap the large 2D colour buffer containing colour values for each pixel in GLFW's window
        glfwPollEvents(); //Check for triggered events and call corresponding functions registered via callback methods
    }
    glfwTerminate(); //Clean/Del all GLFW's resources that were allocated
	_CrtDumpMemoryLeaks();
}