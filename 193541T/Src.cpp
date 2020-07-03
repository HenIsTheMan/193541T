#include "Src.h"
#include "ShaderChief.h"

void framebuffer_size_callback(GLFWwindow*, int width, int height){ //resize callback
    glViewport(0, 0, width, height);
}

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
}

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //For Mac OS X

    GLFWwindow* win = glfwCreateWindow(800, 600, "193541T", 0, 0); //Get a handle to the created window obj
    if(win == 0){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(win); //Make context of the window the main context on the curr thread

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to init GLAD" << std::endl;
        return -1;
    }
    glfwSetFramebufferSizeCallback(win, &framebuffer_size_callback); //??

    ShaderChief shaderMan("Resources/Shaders/Basic.vs", "Resources/Shaders/Basic.fs");
    while(!glfwWindowShouldClose(win)){ //Render loop
        processInput(win);

        glClearColor(0.2f, 0.3f, 0.3f, 1.f); //State-setting function
        glClear(GL_COLOR_BUFFER_BIT); //diff types?? //State-using function

        float vertices[] = {
            // positions         // colors
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom right
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,   // bottom left
            0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // top
        };

        unsigned int indices[] = {
            0, 1, 2
        };

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

        //unsigned int vsID; //...
        //vsID = glCreateShader(GL_VERTEX_SHADER); //...
        //const GLchar* vsSrc = "#version 330 core\n"
        //    "layout (location = 0) in vec3 aPos;\n"
        //    "void main()\n"
        //    "{\n"
        //    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        //    "}\0";
        //glShaderSource(vsID, 1, &vsSrc, NULL);
        //glCompileShader(vsID);
        //int  success;
        //char infoLog[512];
        //glGetShaderiv(vsID, GL_COMPILE_STATUS, &success);
        //if(!success){
        //    glGetShaderInfoLog(vsID, 512, NULL, infoLog);
        //    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        //}

        //unsigned int fsID; //...
        //fsID = glCreateShader(GL_FRAGMENT_SHADER); //...
        //const GLchar* fsSrc = "#version 330 core\n"
        //    "out vec4 FragColor;\n"
        //    "uniform vec4 ourColor;\n"
        //    "void main()\n"
        //    "{\n"
        //    "   FragColor = ourColor; \n"
        //    "}\0";
        //glShaderSource(fsID, 1, &fsSrc, NULL);
        //glCompileShader(fsID);
        //glGetShaderiv(fsID, GL_COMPILE_STATUS, &success);
        //if(!success){
        //    glGetShaderInfoLog(fsID, 512, NULL, infoLog);
        //    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        //}

        //unsigned int shaderProgram;
        //shaderProgram = glCreateProgram();
        //glAttachShader(shaderProgram, vsID);
        //glAttachShader(shaderProgram, fsID);
        //glLinkProgram(shaderProgram);
        //glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        //if(!success){
        //    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        //    std::cout << "ERROR::LINK_FAILED\n" << infoLog << std::endl;
        //}

        //////////////////

        //glUseProgram(shaderProgram);
        //glDeleteShader(vsID);
        //glDeleteShader(fsID);

        float greenValue = ((float)sin(glfwGetTime()) / 2.f) + .5f; //0.f - 1.f
        int vertexColorLocation = glGetUniformLocation(shaderMan.GetShaderProgID(), "ourColor");
        if(vertexColorLocation != -1){
            glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f); //Sets uniform on the currently active shader programme
        }

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        //glDrawArrays(GL_TRIANGLES, 0, 3); //GL_TRIANGLES??
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //Render from index buffer rather than vertex buffer
        glBindVertexArray(0); //Break the existing vertex arr obj binding

        glfwSwapBuffers(win); //Swap the large 2D colour buffer containing colour values for each pixel in GLFW's window
        glfwPollEvents(); //Check for triggered events and call corresponding functions registered via callback methods
    }
    glfwTerminate(); //Clean/Del all GLFW's resources that were allocated
	_CrtDumpMemoryLeaks();
}