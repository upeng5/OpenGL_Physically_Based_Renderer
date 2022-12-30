#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>

Shader::Shader()
{
}

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path)
{
	std::string vertex_source = ParseShader(vertex_path);
	std::string fragment_source = ParseShader(fragment_path);

	const char* vertex_source_pointer = vertex_source.c_str();
	const char* fragment_source_pointer = fragment_source.c_str();

	unsigned int vertex_shader;
	vertex_shader = CompileShader(vertex_source_pointer, GL_VERTEX_SHADER);

	unsigned int fragment_shader;
	fragment_shader = CompileShader(fragment_source_pointer, GL_FRAGMENT_SHADER);

	id_ = glCreateProgram();
	glAttachShader(id_, vertex_shader);
	glAttachShader(id_, fragment_shader);
	glLinkProgram(id_);

	// Check for shader program linking errors
	int program_linked;
	glGetProgramiv(id_, GL_LINK_STATUS, &program_linked);
	if (program_linked != GL_TRUE)
	{
		int log_length = 0;
		char message[1024];
		glGetProgramInfoLog(id_, 1024, &log_length, message);
		std::cout << "ERROR::OPENGL::SHADER::PROGRAM_LINK_FAILED" << std::endl;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void Shader::Bind() const
{
	if (!id_) throw std::exception("Shader::ID_IS_NOT_INITIALIZED");
	glUseProgram(id_);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

unsigned int Shader::GetId() const
{
	return id_;
}

void Shader::SetBool(const std::string& name, bool value)
{
	glUniform1i(GetUniformLocation(name), (int)value);
}

void Shader::SetInt(const std::string& name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string& name, float value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec2f(const std::string& name, const glm::vec2& vector)
{
	glUniform2fv(GetUniformLocation(name), 1, &vector[0]);
}

void Shader::SetVec2f(const std::string& name, float x, float y)
{
	glUniform2f(GetUniformLocation(name), x, y);
}

void Shader::SetVec3f(const std::string& name, const glm::vec3& vector)
{
	glUniform3fv(GetUniformLocation(name), 1, &vector[0]);
}

void Shader::SetVec3f(const std::string& name, float x, float y, float z)
{
	glUniform3f(GetUniformLocation(name), x, y, z);
}

void Shader::SetVec4f(const std::string& name, const glm::vec4& vector)
{
	glUniform4fv(GetUniformLocation(name), 1, &vector[0]);
}

void Shader::SetVec4f(const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(GetUniformLocation(name), x, y, z, w);
}

void Shader::SetMat3f(const std::string& name, const glm::mat3& matrix)
{
	glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::SetMat4f(const std::string& name, const glm::mat4& matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::PreloadShader(const std::string& path)
{
	std::string shader_source = ParseShader(path);
}

std::string Shader::ParseShader(const std::string& path)
{
	std::ifstream file(path);
	std::string line;
	std::string content;
	while (getline(file, line)) {
		content.append(line + "\n");
	}
	return content;
}

unsigned int Shader::CompileShader(const char* source, GLuint type)
{
	int success;
	char infoLog[512];
	std::string shaderType;
	if (type == GL_VERTEX_SHADER) {
		shaderType = "VERTEX";
	}
	else if (type == GL_FRAGMENT_SHADER) {
		shaderType = "FRAGMENT";
	}

	unsigned int shader;
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::OPENGL::SHADER::" << shaderType << "::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	return shader;
}

int Shader::GetUniformLocation(const std::string& name)
{
	int uniform_loc;
	if (uniform_cache_.find(name) != uniform_cache_.end()) {
		uniform_loc = uniform_cache_[name];
	}
	else {
		uniform_loc = glGetUniformLocation(id_, name.c_str());
		uniform_cache_[name] = uniform_loc;
	}

	return uniform_loc;
}

Shader::~Shader()
{
	glDeleteProgram(id_);
}