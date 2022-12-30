#pragma once

#include <vector>
#include <initializer_list>
#include <memory>
#include <glm/glm.hpp>

// offsetof(VertexAttribute, count);

struct VertexAttrib {
	unsigned int count;
	int type;
	unsigned int stride;
};

// An abstraction for OpenGL's vertex buffer layout
class Layout
{
public:
	Layout();
	void Push(unsigned int count, int type, unsigned int stride);
	void Push(const std::initializer_list<VertexAttrib>& list);

	unsigned int Index() const;
	unsigned int Stride() const;
private:
	unsigned int index_;
	unsigned int stride_;
	unsigned int offset_count_;
	unsigned int GetTypeSize(int type);
};
