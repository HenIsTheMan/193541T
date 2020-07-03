#pragma once
#include "Src.h"

class ShaderProg final{
	cstr shaderFilePaths[3];
	static uint currRefID;
	uint refID;
	void Init() noexcept;
	void ParseShader(cstr const&, const uint&) const noexcept;
	void Link() const noexcept;
public:
	ShaderProg(cstr const&, cstr const&, cstr const& = "") noexcept;
	~ShaderProg() noexcept;
	void Use() noexcept;

	///Utility funcs
	static void SetUni1f(cstr const&, const float&, const bool&& = 1) noexcept;
	static void SetUni2f(cstr const&, const float&, const float&, const bool&& = 1) noexcept;
	static void SetUni3f(cstr const&, const float&, const float&, const float&, const bool&& = 1) noexcept;
	static void SetUni3f(cstr const&, const glm::vec3&, const bool&& = 1) noexcept;
	static void SetUni4f(cstr const&, const float[4], const bool&& = 1) noexcept;
	static void SetUni1i(cstr const&, const int&, const bool&& = 1) noexcept;
	static void SetUniMtx4fv(cstr const&, const float* const&, const bool&& = 1) noexcept;
};