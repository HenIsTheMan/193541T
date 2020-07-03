#pragma once
#include "Src.h"

class ShaderChief final{
	uint shaderProgID;
	static uint currID;
	void ParseShader(cstr, uint) const;
	void LinkProg() const;
public:
	ShaderChief(cstr, cstr);
	~ShaderChief();

	///Utility functions
	static void SetUni1f(cstr, float);
	static void SetUniMtx4fv(cstr, float*);
	static void SetUni3f(cstr, float, float, float);
	static void SetUni4f(cstr, float[4]);
	static void SetUni1i(cstr, int);

	void UseProg() const;
};