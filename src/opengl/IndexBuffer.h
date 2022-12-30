#pragma once

#include <glad/glad.h>
#include <vector>

class IndexBuffer
{
public:
	IndexBuffer(std::vector<unsigned int>& indices);
	IndexBuffer(unsigned int* indices, unsigned int count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;
	void Delete() const;

	unsigned int GetId() const;
	unsigned int Count() const;

private:
	unsigned int id_;
	unsigned int count_;
};



