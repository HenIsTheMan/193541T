#pragma once
#include "Src.h"

class ShaderChief final{
	uint shaderProgID;
public:
	ShaderChief(cstr, cstr);
	~ShaderChief();
	uint GetShaderProgID() const;
	void ParseShader(cstr, uint&) const;
	void AttachShader(uint) const;
	void LinkProg() const;
	void UseProg() const;
	void DeleteShader(uint) const;
	void SetUni() const;
};