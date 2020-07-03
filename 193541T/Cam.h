#pragma once
#include "Src.h"

struct Inputs{
	Inputs();
	float upDown, leftRight, frontBack;
};

class Cam final{
	friend int main();
	float spd;
	glm::vec3 pos, target, defaultPos, defaultTarget;
	Inputs inputs;
public:
	Cam(glm::vec3, glm::vec3);
	glm::mat4 LookAt() const;
	glm::vec3 CalcFront(bool = 1) const, CalcRight() const, CalcUp() const;
	void Update(float, float);
	void Reset();
	void SetSpd(float);
	void SetPos(glm::vec3), SetTarget(glm::vec3);

	//++ func overloading
};