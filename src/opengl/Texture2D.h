#pragma once

#include <string>

class Texture2D
{
public:
	Texture2D();
	Texture2D(const std::string& path);
	Texture2D(const std::string& path, int type);
	~Texture2D();

	void Bind(unsigned int slot = 0);
	void Unbind();

	void SetTexture(const std::string& path);

	unsigned int ID() const;
	int Width() const;
	int Height() const;
	int Channels() const;
private:
	unsigned int id_;
	int width_ = 0;
	int height_ = 0;
	int channels_;
	int format_;
	bool initialized_ = false;

	void LoadTexture(const std::string& path);
};
