#define STB_IMAGE_IMPLEMENTATION
#include "Src.h"
#include "App.h"
#include "Cam.h"

int main(){
    srand(uint(glfwGetTime()));
    App* app = new App;
    Cam* cam = new Cam(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f));
    while(!glfwWindowShouldClose(App::win)){ //Main loop
        app->Update();
        cam->Update(GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S);
        app->Render(*cam);
        glfwSwapBuffers(App::win); //Swap the large 2D colour buffer containing colour values for each pixel in GLFW's window
        glfwPollEvents(); //Check for triggered events and call corresponding functions registered via callback methods
    }
    delete app;
    delete cam;
	_CrtDumpMemoryLeaks();
}

//int value = 1;
//uint UBO; //Store global uni vars in a fixed piece of GPU mem
//glGenBuffers(1, &UBO); //Using a UBO increases limit of uni data OpenGL can handle (can query with GL_MAX_VERTEX_UNIFORM_COMPONENTS, good for stuff needing a lot of unis like skeletal animation)
//glBindBuffer(GL_UNIFORM_BUFFER, UBO); {
//    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW); //Size of GLM's mat4 corresponds directly to GLSL's mat4
//    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
//    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
//    glBufferData(GL_UNIFORM_BUFFER, 152, NULL, GL_STATIC_DRAW); //Alloc 152 bytes of mem
//    glBufferSubData(GL_UNIFORM_BUFFER, 144, 4, &value); //Can also add all the data with 1 byte arr //144 is aligned byte offset of 1 of the uni vars declared in a uni block
//} glBindBuffer(GL_UNIFORM_BUFFER, 0);

//uint location = glGetUniformBlockIndex(shaderA.ID, "Lights"); //Indices of active uni blocks in a shader prog are assigned in consecutive/... order starting from 0
//if(location != GL_INVALID_INDEX){
//    glUniformBlockBinding(shaderA.ID, location, 2); //Link uni interface block of shader to a binding pt //Binding pt index: 2 //Uni interface blocks, with the same def, linked to the same binding point share the same uni data from the same UBO
//} else{
//    //??
//}

//glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));
//glBindBufferRange(GL_UNIFORM_BUFFER, 2, UBO, 0, 152); //Bind UBO to the same binding pt //Extra offset and size param as compared to glBindBufferBase

//unsigned int uniformBlockIndexRed = glGetUniformBlockIndex(shaderRed.ID, "Matrices");
//unsigned int uniformBlockIndexGreen = glGetUniformBlockIndex(shaderGreen.ID, "Matrices");
//unsigned int uniformBlockIndexBlue = glGetUniformBlockIndex(shaderBlue.ID, "Matrices");
//unsigned int uniformBlockIndexYellow = glGetUniformBlockIndex(shaderYellow.ID, "Matrices");

//glUniformBlockBinding(shaderRed.ID, uniformBlockIndexRed, 0);
//glUniformBlockBinding(shaderGreen.ID, uniformBlockIndexGreen, 0);
//glUniformBlockBinding(shaderBlue.ID, uniformBlockIndexBlue, 0);
//glUniformBlockBinding(shaderYellow.ID, uniformBlockIndexYellow, 0);