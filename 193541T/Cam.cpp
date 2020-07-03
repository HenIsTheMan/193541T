#include "Cam.h"
#include "App.h"

Cam::Cam(const glm::vec3& newPos, const glm::vec3& newTarget){
	pos = defaultPos = newPos;
	target = defaultTarget = newTarget;
}

glm::mat4 Cam::LookAt() const{ //Translate meshes in the scene
	glm::vec3 vecArr[]{-CalcRight(), CalcUp(), -CalcFront()};
	glm::mat4 translation = glm::mat4(1.0f), rotation = glm::mat4(1.0f);
	for(short i = 0; i < 3; ++i){ //Access elements as mat[col][row] due to column-major order
	    translation[3][i] = -pos[i];
	    for(short j = 0; j < 3; ++j){
	        rotation[i][j] = (vecArr[j])[i];
	    }
	}
	return rotation * translation;
}

glm::vec3 Cam::CalcFront(bool normalised) const{
	return (normalised ? glm::normalize(target - pos) : target - pos);
}

glm::vec3 Cam::CalcRight() const{
	return glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), CalcFront()));
}

glm::vec3 Cam::CalcUp() const{
	return glm::cross(CalcFront(), CalcRight());
}

void Cam::Update(const int& up, const int& down, const int& left, const int& right, const int& front, const int& back){
	float spd = 2.5f * App::dt; //Why cannot double??
	float upDown = float(App::Key(up) - App::Key(down)), leftRight = float(App::Key(left) - App::Key(right)), frontBack = float(App::Key(front) - App::Key(back));
	pos += upDown * spd * CalcUp();
	target += upDown * spd * CalcUp();
	pos += leftRight * spd * CalcRight();
	target += leftRight * spd * CalcRight();
	pos += frontBack * spd * CalcFront();
	target += frontBack * spd * CalcFront();

	float pitch = -glm::radians(App::pitch), yaw = -glm::radians(App::yaw);
	glm::vec3 camFront = target - pos;
	glm::mat4 pitchMat = glm::rotate(glm::mat4(1.f), pitch, CalcRight());
	glm::mat4 yawMat = glm::rotate(glm::mat4(1.f), yaw, CalcUp());
	camFront = (pitchMat * yawMat * glm::vec4(camFront, 999.f));
	target = pos + glm::normalize(camFront);
	App::pitch = App::yaw = 0.f;
	//glm::vec3 camFront(cos(yaw) * cos(pitch), sin(pitch), sin(yaw) * cos(pitch));
}

void Cam::Reset(){
	pos = defaultPos;
	target = defaultTarget;
}

glm::vec3 Cam::GetPos() const{
	return pos;
}

glm::vec3 Cam::GetTarget() const{
	return target;
}

void Cam::SetPos(const glm::vec3& newPos){
	pos = newPos;
}

void Cam::SetTarget(const glm::vec3& newTarget){
	target = newTarget;
}

//const float radius = 10.0f;
//pos.x = (float)sin(glfwGetTime()) * radius;
//pos.z = (float)cos(glfwGetTime()) * radius;

//lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
//lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;

//PTZ, ARC, FOLLOWING, TRACKING

//(float)glfwGetTime() * glm::radians(20.0f), glm::vec3(1.0f, 0.3f, 0.5f)