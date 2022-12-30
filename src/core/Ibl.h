#pragma once

#include <string>
#include <thread>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader;
class Renderer;

// Utiliy functions for Image-Based Lightning
namespace Ibl {
	unsigned int CubemapFromHDRI(const std::string& path, unsigned int& capture_fbo, unsigned int& capture_rbo, Shader& equirectangular_to_cubemap,
		const glm::mat4& capture_projection, const glm::mat4 capture_views[], Renderer& renderer);

	unsigned int CreateIrradianceMap(unsigned int& capture_fbo, unsigned int& capture_rbo, Shader& irradiance_shader, unsigned int env_cubemap,
		const glm::mat4& capture_projection, const glm::mat4 capture_views[], Renderer& renderer);

	unsigned int CreatePrefilterMap(unsigned int& capture_fbo, unsigned int& capture_rbo, Shader& prefilter_shader, unsigned int env_cubemap,
		const glm::mat4& capture_projection, const glm::mat4 capture_views[], Renderer& renderer);

	unsigned int CreateBRDFLookupTexture(unsigned int& capture_fbo, unsigned int& capture_rbo, Shader& brdf_shader, Renderer& renderer);
}