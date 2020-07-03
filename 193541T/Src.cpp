#define STB_IMAGE_IMPLEMENTATION
#include "Src.h"
#include "App.h"
#include "Cam.h"

int main(){
    srand(uint(glfwGetTime()));
    App* app = new App; //Implement Factory Method design pattern??
    Cam* cam = new Cam(glm::vec3(0.f, 0.f, -5.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
    while(!glfwWindowShouldClose(App::win)){ //Main loop
        app->Update(*cam);
        cam->Update(GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S);
        app->Render(*cam);
        glfwSwapBuffers(App::win); //Swap the large 2D colour buffer containing colour values for each pixel in GLFW's window
        glfwPollEvents(); //Check for triggered events and call corresponding functions registered via callback methods
    }
    delete app;
    delete cam;
	_CrtDumpMemoryLeaks();
}