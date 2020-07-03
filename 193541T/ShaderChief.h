#pragma once
#include "Src.h"

class ShaderChief final{
	uint shaderProgID;
	void ParseShader(cstr, uint) const;
	void AttachShader(uint) const;
	void LinkProg() const;
	void UseProg() const;
	void DeleteShader(uint) const;
public:
	ShaderChief(cstr, cstr);
	~ShaderChief();

	///Utility functions
	void SetUni(cstr, float) const;
	void SetUni(cstr, float*) const;
	//void SetUni(cstr, float[4]) const;
	void SetUni(cstr, int) const;
};