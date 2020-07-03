#pragma once
#include "Src.h"

class TexColourBuffer final{
	uint refID;
	void Create(const int&, const int&, const int&, const int&, const int&, const int&, const int&);
public:
	TexColourBuffer(const int&, const int&, const int&, const int&, const int&, const int&, const int&);
	const uint& GetRefID() const noexcept;
};

class Renderbuffer final{
	uint refID;
	void Create(const int&, const int&, const int&);
public:
	Renderbuffer(const int&, const int&, const int&);
	const uint& GetRefID() const noexcept;
};

class Framebuffer final{
	uint refID;
	TexColourBuffer texColourBuffer;
	Renderbuffer* RBO;
public:
	Framebuffer(const int&&, const int&&, const int&&, const int&&, const int&&, const int&&, const int&&);
	void Del();
	const uint& GetRefID() const noexcept;
	const TexColourBuffer& GetTexColourBuffer() const noexcept;
	const Renderbuffer& GetRenderbuffer() const noexcept;
};