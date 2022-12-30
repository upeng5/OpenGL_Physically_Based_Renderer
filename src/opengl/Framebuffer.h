#pragma once

#include <vector>

#include "Texture2D.h"

class Framebuffer
{
public:
	Framebuffer();
	~Framebuffer();

	void Bind();
	void Unbind();

	void AttachDepthBuffer(unsigned int width, unsigned int height);
	void AttachColorBuffer(unsigned int id, int internal_format, int format, unsigned int width, unsigned int height, int index);
	void AttachColorBuffer(Texture2D& texture);

	void Clear();
	void Resize(unsigned int width, unsigned int height);

	void SetID(unsigned int id);

	bool CheckStatus();

private:
	unsigned int id_ = 0;
	unsigned int index_ = 0;
	std::vector<unsigned int> color_attachments_;
	unsigned int depth_attachment_ = 0;
};
