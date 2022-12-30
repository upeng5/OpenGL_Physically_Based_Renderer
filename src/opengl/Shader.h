#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <unordered_map>

class Shader
{
public:
	Shader();
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();
	void Bind() const;
	void Unbind();

	unsigned int GetId() const;

	void SetBool(const std::string& name, bool value);
	void SetInt(const std::string& name, int value);
	void SetFloat(const std::string& name, float value);
	void SetVec2f(const std::string& name, const glm::vec2& vector);
	void SetVec2f(const std::string& name, float x, float y);
	void SetVec3f(const std::string& name, const glm::vec3& vector);
	void SetVec3f(const std::string& name, float x, float y, float z);
	void SetVec4f(const std::string& name, const glm::vec4& vector);
	void SetVec4f(const std::string& name, float x, float y, float z, float w);
	void SetMat3f(const std::string& name, const glm::mat3& matrix);
	void SetMat4f(const std::string& name, const glm::mat4& matrix);
	void PreloadShader(const std::string& path);
private:
	unsigned int id_;
	std::unordered_map<std::string, int> uniform_cache_;

	std::string ParseShader(const std::string& path);
	unsigned int CompileShader(const char* source, GLuint type);
	int GetUniformLocation(const std::string& name);
};
