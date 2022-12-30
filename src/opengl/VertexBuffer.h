#pragma once

#include <glad/glad.h>
#include <vector>
#include <memory>
#include "Layout.h"

class VertexBuffer
{
public:
	VertexBuffer(const std::vector<float>& vertices);
	VertexBuffer(float* vertices, GLsizeiptr size);
	~VertexBuffer();

	void SetLayout(const std::initializer_list<VertexAttrib>& list);

	void Bind() const;
	void Unbind() const;

	unsigned int GetId() const;
private:
	VertexBuffer() = default;
	unsigned int id_;
	Layout layout_;
};
