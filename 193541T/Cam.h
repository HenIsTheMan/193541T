#pragma once
#include "Src.h"
#include "Mesh.h"
#include "ShaderChief.h"

class Cam final{
	glm::vec3 pos, target, defaultPos, defaultTarget;
public:
	friend int main();
	friend void setUniforms(const Cam&, glm::vec3[]);
	Cam(const glm::vec3&, const glm::vec3&);
	glm::vec3 CalcFront(bool = 1) const, CalcRight() const, CalcUp() const;
	glm::mat4 LookAt() const;
	void Update(const int&, const int&, const int&, const int&, const int&, const int&);
	void Reset();
};