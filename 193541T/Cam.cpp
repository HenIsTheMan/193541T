#include "Cam.h"

Inputs::Inputs(){
	upDown = leftRight = frontBack = 0.f;
}

Cam::Cam(glm::vec3 newPos, glm::vec3 newTarget): spd(0.f){
	pos = defaultPos = newPos;
	target = defaultTarget = newTarget;
}

glm::mat4 Cam::LookAt() const{ //Translate the scene
	glm::vec3 back = -CalcFront(), right = glm::normalize(glm::cross(glm::normalize(glm::vec3(0.f, 1.f, 0.f)), back)), up = glm::cross(back, right); //??
	glm::vec3 vecArr[]{right, up, back};
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
	return glm::cross(CalcRight(), CalcFront());
}

void Cam::Update(float pitch, float yaw){
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)); //why??
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)); //why??
	target = pos + glm::normalize(front);

	pos += inputs.upDown * spd * CalcUp();
	target += inputs.upDown * spd * CalcUp();
	pos += inputs.leftRight * spd * CalcRight();
	target += inputs.leftRight * spd * CalcRight();
	pos += inputs.frontBack * spd * CalcFront();
	target += inputs.frontBack * spd * CalcFront();

	//pos = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f)) * pos;
}

void Cam::Reset(){
	pos = defaultPos;
	target = defaultTarget;
}

void Cam::SetSpd(float newSpd){
	spd = newSpd;
}

void Cam::SetPos(glm::vec3 newPos){
	pos = newPos;
}

void Cam::SetTarget(glm::vec3 newTarget){
	target = newTarget;
}

//const float radius = 10.0f;
/////why??
//cameraPos.x = (float)sin(glfwGetTime()) * radius;
//cameraPos.z = (float)cos(glfwGetTime()) * radius;

//((float)sin(glfwGetTime()) / 2.f) + .5f /*0.f - 1.f*/

//glm::mat4 trans = glm::mat4(1.0f);
//trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0)); //why??
//trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

//PTZ, ARC, FOLLOWING, TRACKING