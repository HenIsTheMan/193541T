#pragma once
#include "Src.h"
#include "Scene.h"

class App final{
	friend class Cam;
	friend void FramebufferSizeCallback(GLFWwindow*, int, int);
	friend void ScrollCallback(GLFWwindow*, double, double);
	friend void CursorPosCallback(GLFWwindow*, double, double);
	float lastFrame;
	float cullBT;
	float polyModeBT;
	Scene* scene;
	static bool firstCall;
	static float pitch, yaw; //Euler angles
	static float lastX, lastY;
	static float sensitivity;
	static bool Key(int);
	uint frontFBO;
	uint backFBO;
	uint enFBO, enTex;
	uint intermediateFBO, intermediateTex;
	void CreateFramebuffer(uint&, uint&);
	void CreateMultisampleFramebuffer(uint&);
public:
	App();
	~App();
	static float dt;
	static GLFWwindow* win;
	void Init(), Update(), Render(const Cam&);
};