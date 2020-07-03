#include "ShaderChief.h"

uint ShaderChief::currID = 0;

ShaderChief::ShaderChief(cstr vsPath, cstr fsPath): shaderProgID(glCreateProgram()){
	uint vsRefID = glCreateShader(GL_VERTEX_SHADER), fsRefID = glCreateShader(GL_FRAGMENT_SHADER);
	ParseShader(vsPath, vsRefID);
	ParseShader(fsPath, fsRefID);
	glAttachShader(shaderProgID, vsRefID);
	glAttachShader(shaderProgID, fsRefID);
	LinkProg();
	glDeleteShader(vsRefID);
	glDeleteShader(fsRefID);
}

ShaderChief::~ShaderChief(){
	glDeleteProgram(shaderProgID);
}

void ShaderChief::ParseShader(cstr fPath, uint shaderID) const{
	int infoLogLength;
	str srcCodeStr, line;
	std::ifstream stream(fPath);

	if(!stream.is_open()){
		printf("Failed to open and read \"%s\"\n", fPath);
		return;
	}
	while(getline(stream, line)){
		srcCodeStr += "\n" + line;
	}
	stream.close();

	printf("Compiling \"%s\"...\n", fPath);
	cstr srcCodeCStr = srcCodeStr.c_str();
	glShaderSource(shaderID, 1, &srcCodeCStr, 0);
	glCompileShader(shaderID);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength){
		char* errorMsg = (char*)_malloca(infoLogLength * sizeof(char)); //Allocate memory on the stack dynamically
		glGetShaderInfoLog(shaderID, infoLogLength, &infoLogLength, errorMsg);
		printf("Failed to compile \"%s\"!\n%s\n", fPath, errorMsg);
	}
}

void ShaderChief::LinkProg() const{
	int infoLogLength;
	printf("Linking programme...\n\n");
	glLinkProgram(shaderProgID); //Vars in diff shaders are linked here too

	glGetProgramiv(shaderProgID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength){
		char* errorMsg = (char*)_malloca(infoLogLength * sizeof(char)); //Allocate memory on the stack dynamically
		glGetProgramInfoLog(shaderProgID, infoLogLength, &infoLogLength, errorMsg);
		printf("%s\n", errorMsg);
	}
}

void ShaderChief::SetUni1f(cstr uniName, float value){
	int uniLocation = glGetUniformLocation(currID, uniName); //Query location of uniform
	if(uniLocation != -1){
		glUniform1f(uniLocation, value); //Sets uniform on the currently active shader prog
	} else{
		printf("%s: Failed to find '%s'\n", std::to_string(currID).c_str(), uniName);
	}
}

void ShaderChief::SetUniMtx4fv(cstr uniName, float* address){
	int uniLocation = glGetUniformLocation(currID, uniName); //Query location of uniform
	if(uniLocation != -1){
		glUniformMatrix4fv(uniLocation, 1, GL_FALSE, address); //Sets uniform on the currently active shader prog
	} else{
		printf("%s: Failed to find '%s'\n", std::to_string(currID).c_str(), uniName);
	}
}

void ShaderChief::SetUni3f(cstr uniName, float value1, float value2, float value3){
	int uniLocation = glGetUniformLocation(currID, uniName); //Query location of uniform
	if(uniLocation != -1){
		glUniform3f(uniLocation, value1, value2, value3); //Sets uniform on the currently active shader prog
	} else{
		printf("%s: Failed to find '%s'\n", std::to_string(currID).c_str(), uniName);
	}
}

void ShaderChief::SetUni4f(cstr uniName, float values[4]){
	int uniLocation = glGetUniformLocation(currID, uniName); //Query location of uniform
	if(uniLocation != -1){
		glUniform4f(uniLocation, values[0], values[1], values[2], values[3]); //Sets uniform on the currently active shader prog
	} else{
		printf("%s: Failed to find '%s'\n", std::to_string(currID).c_str(), uniName);
	}
}

void ShaderChief::SetUni1i(cstr uniName, int value){
	int uniLocation = glGetUniformLocation(currID, uniName); //Query location of uniform
	if(uniLocation != -1){
		glUniform1i(uniLocation, value); //Sets uniform on the currently active shader prog
	} else{
		printf("%s: Failed to find '%s'\n", std::to_string(currID).c_str(), uniName);
	}
}

void ShaderChief::UseProg() const{
	glUseProgram(shaderProgID);
	currID = shaderProgID;
}