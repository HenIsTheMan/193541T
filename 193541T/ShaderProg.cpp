#include "ShaderProg.h"
using std::ifstream;

uint ShaderProg::currRefID;

ShaderProg::ShaderProg(cstr const& vsPath, cstr const& fsPath, cstr const& gsPath) noexcept: refID(999){
	shaderFilePaths[0] = vsPath;
	shaderFilePaths[1] = fsPath;
	shaderFilePaths[2] = gsPath;
}

ShaderProg::~ShaderProg() noexcept{
	glDeleteProgram(refID);
}

void ShaderProg::Init() noexcept{
	refID = glCreateProgram();
	std::vector<uint> shaderRefIDs{glCreateShader(GL_VERTEX_SHADER), glCreateShader(GL_FRAGMENT_SHADER)};
	if(shaderFilePaths[2] != ""){
		shaderRefIDs.emplace_back(glCreateShader(GL_GEOMETRY_SHADER)); //Takes set of vertices that form a primitive as input (so its input data from the vertex shader is always represented as arrays of vertex data even though we only have a single vertex right now??)
	} //Shapes are dynamically generated on the GPU with geometry shaders (better than defining shapes within vertex buffers) so good for simple repeating forms like cubes in a voxel world or grass in a field
	for(short i = 0; i < shaderRefIDs.size(); ++i){
		ParseShader(shaderFilePaths[i], shaderRefIDs[i]);
		glAttachShader(refID, shaderRefIDs[i]);
	}
	Link();
	for(const auto& shaderRefID: shaderRefIDs){
		glDeleteShader(shaderRefID);
	}
}

void ShaderProg::ParseShader(cstr const& fPath, const uint& shaderID) const noexcept{
	int infoLogLength;
	str srcCodeStr, line;
	ifstream stream(fPath);

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

void ShaderProg::Link() const noexcept{
	int infoLogLength;
	printf("Linking programme...\n\n");
	glLinkProgram(refID); //Vars in diff shaders are linked here too

	glGetProgramiv(refID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength){
		char* errorMsg = (char*)_malloca(infoLogLength * sizeof(char)); //Allocate memory on the stack dynamically
		glGetProgramInfoLog(refID, infoLogLength, &infoLogLength, errorMsg);
		printf("%s\n", errorMsg);
	}
}

void ShaderProg::Use() noexcept{
	if(refID == 999){
		Init();
	}
	if(currRefID != refID){
		glUseProgram(refID);
		currRefID = refID;
	}
}

void ShaderProg::SetUni1f(cstr const& uniName, const float& value, const bool&& warn) noexcept{
	int uniLocation = glGetUniformLocation(currRefID, uniName); //Query location of uniform
	if(uniLocation != -1){
		glUniform1f(uniLocation, value); //Sets uniform on the currently active shader prog
	} else if(warn){
		printf("%s: Failed to find '%s'\n", std::to_string(currRefID).c_str(), uniName);
	}
}

void ShaderProg::SetUni2f(cstr const& uniName, const float& value1, const float& value2, const bool&& warn) noexcept{
	int uniLocation = glGetUniformLocation(currRefID, uniName); //Query location of uniform
	if(uniLocation != -1){
		glUniform2f(uniLocation, value1, value2); //Sets uniform on the currently active shader prog
	} else if(warn){
		printf("%s: Failed to find '%s'\n", std::to_string(currRefID).c_str(), uniName);
	}
}

void ShaderProg::SetUni3f(cstr const& uniName, const float& value1, const float& value2, const float& value3, const bool&& warn) noexcept{
	int uniLocation = glGetUniformLocation(currRefID, uniName); //Query location of uniform
	if(uniLocation != -1){
		glUniform3f(uniLocation, value1, value2, value3); //Sets uniform on the currently active shader prog
	} else if(warn){
		printf("%s: Failed to find '%s'\n", std::to_string(currRefID).c_str(), uniName);
	}
}

void ShaderProg::SetUni3f(cstr const& uniName, const glm::vec3& vec, const bool&& warn) noexcept{
	int uniLocation = glGetUniformLocation(currRefID, uniName); //Query location of uniform
	if(uniLocation != -1){
		glUniform3f(uniLocation, vec.x, vec.y, vec.z); //Sets uniform on the currently active shader prog
	} else if(warn){
		printf("%s: Failed to find '%s'\n", std::to_string(currRefID).c_str(), uniName);
	}
}

void ShaderProg::SetUni4f(cstr const& uniName, const float values[4], const bool&& warn) noexcept{
	int uniLocation = glGetUniformLocation(currRefID, uniName); //Query location of uniform
	if(uniLocation != -1){
		glUniform4f(uniLocation, values[0], values[1], values[2], values[3]); //Sets uniform on the currently active shader prog
	} else if(warn){
		printf("%s: Failed to find '%s'\n", std::to_string(currRefID).c_str(), uniName);
	}
}

void ShaderProg::SetUni1i(cstr const& uniName, const int& value, const bool&& warn) noexcept{
	int uniLocation = glGetUniformLocation(currRefID, uniName); //Query location of uniform
	if(uniLocation != -1){
		glUniform1i(uniLocation, value); //Sets uniform on the currently active shader prog
	} else if(warn){
		printf("%s: Failed to find '%s'\n", std::to_string(currRefID).c_str(), uniName);
	}
}

void ShaderProg::SetUniMtx4fv(cstr const& uniName, const float* const& address, const bool&& warn) noexcept{
	int uniLocation = glGetUniformLocation(currRefID, uniName); //Query location of uniform
	if(uniLocation != -1){
		glUniformMatrix4fv(uniLocation, 1, GL_FALSE, address); //Sets uniform on the currently active shader prog
	} else if(warn){
		printf("%s: Failed to find '%s'\n", std::to_string(currRefID).c_str(), uniName);
	}
}