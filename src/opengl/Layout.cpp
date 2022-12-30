#include "Layout.h"

#include <glad/glad.h>

Layout::Layout()
	: index_(0), stride_(0), offset_count_(0)
{
}

unsigned int Layout::Index() const
{
	return index_;
}

unsigned int Layout::Stride() const
{
	return stride_;
}

void Layout::Push(unsigned int count, int type, unsigned int stride)
{
	unsigned int size = GetTypeSize(type);
	unsigned int offset = offset_count_ * size;
	glEnableVertexAttribArray(index_);
	glVertexAttribPointer(index_, count, type, GL_FALSE, stride * size, (void*)offset);
	index_++;
	offset_count_ += count;
}

void Layout::Push(const std::initializer_list<VertexAttrib>& list)
{
	for (const VertexAttrib& attrib : list) {
		unsigned int size = GetTypeSize(attrib.type);
		unsigned int offset = offset_count_ * size;
		glEnableVertexAttribArray(index_);
		glVertexAttribPointer(index_, attrib.count, attrib.type, GL_FALSE, attrib.stride * size, (void*)offset);
		index_++;
		offset_count_ += attrib.count;
	}
}

unsigned int Layout::GetTypeSize(int type)
{
	// Could possibily be precomputed
	switch (type)
	{
	case GL_FLOAT:
		return sizeof(GLfloat);
	case GL_INT:
		return sizeof(GLint);
	case GL_UNSIGNED_INT:
		return sizeof(GLuint);
	case GL_UNSIGNED_BYTE:
		return sizeof(GLubyte);
	default:
		break;
	}
	return 0;
}