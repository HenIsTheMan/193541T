#pragma once
#include "Src.h"

class ShaderChief final{
	uint shaderProgID;
	void ParseShader(cstr, uint) const;
	void AttachShader(uint) const;
	void LinkProg() const;

	void DeleteShader(uint) const;
public:
	ShaderChief(cstr, cstr);
	~ShaderChief();

	void UseProg() const;

	///Utility functions
	void SetUni1f(cstr, float) const;
	void SetUniMtx4fv(cstr, float*) const;
	void SetUni3f(cstr, float, float, float) const;
	void SetUni4f(cstr, float[4]) const;
	void SetUni1i(cstr, int) const;
};