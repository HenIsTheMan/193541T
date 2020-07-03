#pragma once
#include "Src.h"
#include "Mesh.h"
#include "ShaderChief.h"

class Cam final{
	friend class Scene;
	glm::vec3 pos, target, defaultPos, defaultTarget;
public:
	Cam(const glm::vec3&, const glm::vec3&);
	glm::vec3 CalcFront(bool = 1) const, CalcRight() const, CalcUp() const;
	glm::mat4 LookAt() const;
	void Update(const int&, const int&, const int&, const int&, const int&, const int&);
	void Reset();
};