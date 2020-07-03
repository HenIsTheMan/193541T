#pragma once
#include "Src.h"

class App final{
	friend class Cam;
	friend void FramebufferSizeCallback(GLFWwindow*, int, int);
	friend void ScrollCallback(GLFWwindow*, double, double);
	friend void CursorPosCallback(GLFWwindow*, double, double);
	float lastFrame;
	static bool firstCall;
	static float pitch, yaw; //Euler angles //++roll
	static float lastX, lastY;
	static float sensitivity;
	static bool Key(int);
public:
	App();
	~App();
	static float dt;
	static float FOV;
	static GLFWwindow* win;
	void Init(), Update();
};