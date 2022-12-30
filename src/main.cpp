#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <memory>

#include "opengl/Shader.h"
#include "opengl/VertexArray.h"
#include "opengl/VertexBuffer.h"
#include "opengl/IndexBuffer.h"
#include "opengl/Texture2D.h"
#include "core/Camera.h"

#include <glm/gtc/type_ptr.hpp>

#ifdef _DEBUG
#include "Debug.h"
#endif
#include <stb_image.h>
#include "gui/GUI.h"
#include "core/Renderer.h"
#include "core/Ibl.h"

/* CONSTANTS */
// 1 640*480
// 2 1280*720
// 3 1920*1080
const unsigned int kWidth = 1280;
const unsigned int kHeight = 720;

/* CALLBACKS */
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xposIn, double yposIn);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

// Helpers
void ProcessInput(GLFWwindow* window);

// GLOBALS
Camera camera(glm::vec3(0.0f, 1.2f, 4.0f));
Renderer renderer;

// Gui settings
bool on_change = false;

// IBL
unsigned int env_cubemap = 0;
unsigned int irradiance_map = 0;
unsigned int prefilter_map = 0;
unsigned int brdf_lut_texture = 0;

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = kWidth / 2.0;
float lastY = kHeight / 2.0;
float fov = 45.0f;

// timing
float delta_time = 0.0f;	// time between current frame and last frame
float last_frame = 0.0f;

// TODO: Check whether there is a model that can be rendered
bool model_avaliable = false;

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit()) {
		std::cout << "ERROR::GLFW::GLFW_INITIALIZATION_FAILED" << std::endl;
		return -1;
	}

	/* GLFW hints */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef _DEBUG
	// The debug context must be enabled right after initializing the windowing system
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(kWidth, kHeight, "PBR Renderer", nullptr, nullptr);
	if (!window)
	{
		std::cout << "ERROR::GLFW::WINDOW_CREATION_FAILED" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Initialize GLAD */
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "ERROR::GLAD::FAILED_TO_INITIALIZE_OPENGL_CONTEXT" << std::endl;
		return -1;
	}

#ifdef _DEBUG
	//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(DebugOutputCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	else {
		std::cout << "ERROR::OPENGL::FAILED_TO_ENABLE_DEBUG_OUTPUT" << "\n" << std::endl;
	}
#endif // _DEBUG

	/* Configure global OpenGL state */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glEnable(GL_CULL_FACE);

	/* GLFW Callbacks */
	glfwSetErrorCallback(ErrorCallback);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetFramebufferSizeCallback(window, FrameBufferResizeCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	/*  */
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

#ifdef _DEBUG
	std::cout << "Version:  " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Vendor:   " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << std::endl;
#endif

	/* Load shaders */
	Shader shader("shaders/pbr.vert", "shaders/pbr.frag");
	Shader equirectangularToCubemapShader("shaders/hdrmap.vert", "shaders/hdrmap.frag");
	Shader irradiance_shader("shaders/irradiance.vert", "shaders/irradiance.frag");
	Shader prefilter_shader("shaders/irradiance.vert", "shaders/prefilter.frag");
	Shader brdf_shader("shaders/brdf.vert", "shaders/brdf.frag");
	Shader skyboxShader("shaders/skybox.vert", "shaders/skybox.frag");

	shader.Bind();
	shader.SetInt("irradiance_map", 0);
	shader.SetInt("prefilter_map", 1);
	shader.SetInt("brdfLUT", 2);

	// Material textures
	shader.SetInt("albedoMap", 3);
	shader.SetInt("normalMap", 4);
	shader.SetInt("metallicMap", 5);
	shader.SetInt("roughnessMap", 6);

	//shader.SetVec3f("albedo", 0.5f, 0.0f, 0.0f);
	shader.SetFloat("ao", 1.0f);

	equirectangularToCubemapShader.Bind();
	equirectangularToCubemapShader.SetInt("equirectangularMap", 0);

	skyboxShader.Bind();
	skyboxShader.SetInt("environmentMap", 0);

	// lights
	// ------
	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};
	glm::vec3 lightColors[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};
	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 2.5;

	/* Load models */
	/*Model external_model("res/models/AntiqueCamera/glTF/AntiqueCamera.gltf");*/

	/* Add textures here */
	Texture2D hdr_map("res/textures/hdr/satara_night_no_lamps_1k.hdr", GL_RGBA16F);
	// Load PBR Material textures
	Texture2D albedo_map("res/textures/Aluminum-Scuffed_Unreal-Engine/Aluminum-Scuffed_basecolor.png");
	Texture2D normal_map("res/textures/Aluminum-Scuffed_Unreal-Engine/Aluminum-Scuffed_normal.png");
	Texture2D metallic_map("res/textures/Aluminum-Scuffed_Unreal-Engine/Aluminum-Scuffed_metallic.png");
	Texture2D roughness_map("res/textures/Aluminum-Scuffed_Unreal-Engine/Aluminum-Scuffed_roughness.png");

	Texture2D floor_albedo_map("res/textures/oxidized-copper-ue/oxidized-copper-albedo.png");
	Texture2D floor_normal_map("res/textures/oxidized-copper-ue/oxidized-copper-normal-ue.png");
	Texture2D floor_metallic_map("res/textures/oxidized-copper-ue/oxidized-copper-metal.png");
	Texture2D floor_roughness_map("res/textures/oxidized-copper-ue/oxidized-coppper-roughness.png");

	// Convert equirectangular map to cubemap
	unsigned int capture_fbo = 0, capture_rbo = 0;
	glm::mat4 capture_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 capture_views[] =
	{
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	env_cubemap = Ibl::CubemapFromHDRI("res/textures/hdr/satara_night_no_lamps_1k.hdr", capture_fbo, capture_rbo, equirectangularToCubemapShader, capture_projection, capture_views, renderer);

	// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
	// --------------------------------------------------------------------------------
	irradiance_map = Ibl::CreateIrradianceMap(capture_fbo, capture_fbo, irradiance_shader, env_cubemap, capture_projection, capture_views, renderer);

	// pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
	// --------------------------------------------------------------------------------
	prefilter_map = Ibl::CreatePrefilterMap(capture_fbo, capture_fbo, prefilter_shader, env_cubemap, capture_projection, capture_views, renderer);

	// pbr: generate a 2D LUT from the BRDF equations used.
	// ----------------------------------------------------
	brdf_lut_texture = Ibl::CreateBRDFLookupTexture(capture_fbo, capture_rbo, brdf_shader, renderer);

	// initialize static shader uniforms before rendering
	// --------------------------------------------------
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)kWidth / (float)kHeight, 0.1f, 100.0f);
	shader.Bind();
	shader.SetMat4f("projection", projection);
	skyboxShader.Bind();
	skyboxShader.SetMat4f("projection", projection);

	// then before rendering, configure the viewport to the original framebuffer's screen dimensions
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	glViewport(0, 0, w, h);

	/* Initialize GUI */
	GUI gui(window);

	Renderer renderer;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ProcessInput(window);

		// Start the ImGui frame
		gui.Initialize();

		shader.Bind();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.SetMat4f("view", view);
		shader.SetVec3f("camPos", camera.Position);

		// Bind Material textures
		floor_albedo_map.Bind(3);
		floor_normal_map.Bind(4);
		floor_metallic_map.Bind(5);
		floor_roughness_map.Bind(6);

		model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f));
		model = glm::rotate(model, (float)glm::radians(90.f), glm::vec3(1.0, 0.0, 0.0));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f));
		shader.SetMat4f("view", view);
		shader.SetMat4f("model", model);
		renderer.DrawCube();

		model = glm::mat4(1.0f);

		// Bind Material textures
		albedo_map.Bind(3);
		normal_map.Bind(4);
		metallic_map.Bind(5);
		roughness_map.Bind(6);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));
		shader.SetMat4f("model", model);
		renderer.DrawSphere();

		if (on_change) {
			glDeleteTextures(1, &env_cubemap);
			glDeleteTextures(1, &irradiance_map);
			glDeleteTextures(1, &prefilter_map);
			glDeleteTextures(1, &brdf_lut_texture);

			glDeleteRenderbuffers(1, &capture_rbo);
			glDeleteFramebuffers(1, &capture_fbo);

			env_cubemap = Ibl::CubemapFromHDRI(gui.settings_->ibl_map_path, capture_fbo, capture_rbo, equirectangularToCubemapShader, capture_projection, capture_views, renderer);
			irradiance_map = Ibl::CreateIrradianceMap(capture_fbo, capture_fbo, irradiance_shader, env_cubemap, capture_projection, capture_views, renderer);
			prefilter_map = Ibl::CreatePrefilterMap(capture_fbo, capture_fbo, prefilter_shader, env_cubemap, capture_projection, capture_views, renderer);
			brdf_lut_texture = Ibl::CreateBRDFLookupTexture(capture_fbo, capture_rbo, brdf_shader, renderer);

			glfwGetWindowSize(window, &w, &h);
			glViewport(0, 0, w, h);

			//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

			on_change = false;
		}

		// bind pre-computed IBL data
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance_map);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter_map);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, brdf_lut_texture);

		// Sphere rendering was here

		// Render Skybox
		skyboxShader.Bind();
		skyboxShader.SetMat4f("view", view);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, env_cubemap);
		renderer.DrawCube();

		// Render GUI here
		gui.Render(on_change);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	// Destroy window & GUI
	gui.Destroy();

	glDeleteRenderbuffers(1, &capture_rbo);
	glDeleteFramebuffers(1, &capture_fbo);

	glDeleteTextures(1, &env_cubemap);
	glDeleteTextures(1, &irradiance_map);
	glDeleteTextures(1, &prefilter_map);
	glDeleteTextures(1, &brdf_lut_texture);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void ErrorCallback(int error, const char* description)
{
	std::cerr << "ERROR::GLFW\n" << description << std::endl;
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void FrameBufferResizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		return;
	}

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	camera.Front = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

void ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = static_cast<float>(camera.MovementSpeed * delta_time);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.Position += cameraSpeed * camera.Front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.Position -= cameraSpeed * camera.Front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.Position -= glm::normalize(glm::cross(camera.Front, camera.Up)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.Position += glm::normalize(glm::cross(camera.Front, camera.Up)) * cameraSpeed;
}