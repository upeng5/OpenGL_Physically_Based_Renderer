#include "VertexArray.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &id_);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &id_);
}

void VertexArray::Bind() const
{
	glBindVertexArray(id_);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}

unsigned int VertexArray::GetId() const
{
	return id_;
}