#include "IndexBuffer.h"

#include <iostream>

IndexBuffer::IndexBuffer(std::vector<unsigned int>& indices)
{ 
	glGenBuffers(1, &id_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	count_ = static_cast<unsigned int>(indices.size());
}

IndexBuffer::IndexBuffer(unsigned int* indices, unsigned int count)
{
	glGenBuffers(1, &id_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	count_ = count;
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &id_);
}

void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
}

void IndexBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::Delete() const
{
	glDeleteBuffers(1, &id_);
}

unsigned int IndexBuffer::GetId() const
{
	return id_;
}

unsigned int IndexBuffer::Count() const
{
	return count_;
}
