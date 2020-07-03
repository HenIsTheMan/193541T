#include "Cam.h"
#include "App.h"

extern float dt;
extern float leftRightMB;
extern float pitch;
extern float yaw;

Cam::Cam(const glm::vec3& newPos, const glm::vec3& newTarget, const short&& newProjectionType, const float&& newAspectRatio):
	pos(newPos), target(newTarget),
	defaultPos(newPos), defaultTarget(newTarget),
	projectionType(newProjectionType),
	aspectRatio(newAspectRatio){}

glm::mat4 Cam::LookAt() const{ //Translate meshes in the scene
	glm::vec3 vecArr[]{CalcRight(), CalcUp(), -CalcFront()};
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

glm::vec3 Cam::CalcRight() const{ //Prevent LookAt flip once front vec parallel to worldUp vec??
	return glm::normalize(glm::cross(CalcFront(), {0.f, 1.f, 0.f}));

	//glm::vec3 camFront = CalcFront();
	//glm::vec3 result = glm::normalize(glm::cross(camFront, {0.f, 1.f, 0.f}));	
	//if(result == glm::vec3(0.f)){
	//	/*const_cast<glm::vec3&>(prevRight) = result;*/
	//	glm::vec3 camUp = glm::cross({1.f, 0.f, 0.f}, camFront);
	//	return glm::normalize(glm::cross(camFront, camUp));
	//}
	//return result;
}

glm::vec3 Cam::CalcUp() const{
	return glm::normalize(glm::cross(CalcRight(), CalcFront()));
}

void Cam::Update(const int& up, const int& down, const int& left, const int& right, const int& front, const int& back){
	const float camSpd = 4.f * dt;
	float upDown = float(App::Key(up) - App::Key(down));
	float leftRight = float(App::Key(left) - App::Key(right));
	float frontBack = float(App::Key(front) - App::Key(back));

	const glm::vec3&& camFront = CalcFront();
	const glm::vec3&& xzCamFront = glm::vec3(camFront.x, 0.f, camFront.z);
	glm::vec3&& frontBackDir = glm::normalize(glm::dot(camFront, glm::normalize(xzCamFront)) * glm::normalize(xzCamFront));
	frontBackDir.y = 1.f;

	glm::vec3&& change = glm::vec3(frontBack, upDown, frontBack) * frontBackDir + leftRight * -CalcRight() + leftRightMB * camFront;
	change = glm::vec3(std::min(1.f, std::max(-1.f, change.x)), std::min(1.f, std::max(-1.f, change.y)), std::min(1.f, std::max(-1.f, change.z)));
	pos += camSpd * change;
	target += camSpd * change;

	glm::mat4 yawPitch = glm::rotate(glm::rotate(glm::mat4(1.f), glm::radians(yaw), {0.f, 1.f, 0.f}), glm::radians(pitch), CalcRight());
	target = pos + glm::vec3(yawPitch * glm::vec4(camFront, 0.f));
	yaw = pitch = 0.f;
}

void Cam::Reset(){
	projectionType = 1;
	pos = defaultPos;
	target = defaultTarget;
}

const float& Cam::GetAspectRatio() const{
	return aspectRatio;
}

const short& Cam::GetProjectionType() const{
	return projectionType;
}

const glm::vec3& Cam::GetPos() const{
	return pos;
}

const glm::vec3& Cam::GetTarget() const{
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
//lightPos.y = sin(glfwGetTime() / 2.0f);

//PTZ, ARC, FOLLOWING, TRACKING, FLIGHT

//(float)glfwGetTime() * glm::radians(20.0f), glm::vec3(1.0f, 0.3f, 0.5f)