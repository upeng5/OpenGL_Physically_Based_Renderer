#include "Framebuffer.h"

#include <glad/glad.h>

Framebuffer::Framebuffer()
	: id_{ 0 }
{
	glGenFramebuffers(1, &id_);
	glBindFramebuffer(GL_FRAMEBUFFER, id_);
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &id_);
	glDeleteTextures(color_attachments_.size(), color_attachments_.data());
	glDeleteTextures(1, &depth_attachment_);
}

void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, id_);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::AttachDepthBuffer(unsigned int width, unsigned int height)
{
	glGenRenderbuffers(1, &depth_attachment_);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_attachment_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_attachment_);
}

void Framebuffer::AttachColorBuffer(unsigned int id, int internal_format, int format, unsigned int width, unsigned int height, int index)
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	// TODO - Make the data type changeable throught the parameter
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index_, GL_TEXTURE_2D, id, 0);
	color_attachments_.push_back(id);
	index_++;
}

void Framebuffer::AttachColorBuffer(Texture2D& texture)
{
	texture.Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index_, GL_TEXTURE_2D, texture.ID(), 0);
	color_attachments_.push_back(texture.ID());
	index_++;
}

void Framebuffer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Framebuffer::Resize(unsigned int width, unsigned int height)
{
	glViewport(0, 0, width, height);
}

bool Framebuffer::CheckStatus()
{
	return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}