#include "ShaderChief.h"

ShaderChief::ShaderChief(cstr vsPath, cstr fsPath): shaderProgID(glCreateProgram()){
	uint vsID = glCreateShader(GL_VERTEX_SHADER), fsID = glCreateShader(GL_FRAGMENT_SHADER);
	ParseShader(vsPath, vsID);
	ParseShader(fsPath, fsID);
	AttachShader(vsID);
	AttachShader(fsID);
	LinkProg();
	UseProg();
	DeleteShader(vsID);
	DeleteShader(fsID);
}

ShaderChief::~ShaderChief(){
	glDeleteProgram(shaderProgID);
}

uint ShaderChief::GetShaderProgID() const{
	return shaderProgID;
}

void ShaderChief::ParseShader(cstr fPath, uint& shaderID) const{
	int infoLogLength;
	str srcCodeStr, line;
	std::ifstream stream(fPath);

	if(stream.is_open()){
		while(getline(stream, line)){
			srcCodeStr += "\n" + line;
		}
		stream.close();
	} else{
		printf("Failed to open and read \"%s\"\n", fPath);
		return;
	}

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

void ShaderChief::AttachShader(uint shaderID) const{
	glAttachShader(shaderProgID, shaderID);
}

void ShaderChief::LinkProg() const{
	int infoLogLength;
	printf("Linking programme...");
	glLinkProgram(shaderProgID); //Vars in diff shaders are linked here too

	glGetProgramiv(shaderProgID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength){
		char* errorMsg = (char*)_malloca(infoLogLength * sizeof(char)); //Allocate memory on the stack dynamically
		glGetProgramInfoLog(shaderProgID, infoLogLength, &infoLogLength, errorMsg);
		printf("%s\n", errorMsg);
	}
}

void ShaderChief::UseProg() const{
	glUseProgram(shaderProgID);
}

void ShaderChief::DeleteShader(uint shaderID) const{
	glDeleteShader(shaderID);
}

void ShaderChief::SetUni() const{}