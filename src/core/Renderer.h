#pragma once

#include "../opengl/VertexArray.h"
#include "../opengl/IndexBuffer.h"

class Shader;

class Renderer
{
public:
	void Clear();
	void DrawIndexed(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const;
	void Draw();

	void DrawSphere();
	void DrawCube();
	void DrawQuad();

	~Renderer();
private:
	unsigned int index_count_;
	unsigned int spherevao_ = 0, spherevbo_ = 0;
	unsigned int cubevao_ = 0, cubevbo_ = 0;
	unsigned int quadvao_ = 0, quadvbo_ = 0;
};
