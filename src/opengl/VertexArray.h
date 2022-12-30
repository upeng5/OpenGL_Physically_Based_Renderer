#pragma once

#include "VertexBuffer.h"

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void Bind() const;
	void Unbind() const;

	unsigned int GetId() const;
private:
	unsigned int id_;
};
