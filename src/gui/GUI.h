#pragma once

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <ImGuiFileDialogConfig.h>
#include <ImGuiFileDialog.h>

#include <memory>

struct Settings {
	std::string ibl_map_path = "";
	std::string display_ibl_path = "";
	std::string model_path = "";
};

class GUI
{
public:
	GUI(GLFWwindow* window);
	void Initialize();
	void Render(bool& on_change);
	void Render(std::unique_ptr<Settings> settings);
	void Destroy();

public:
	std::unique_ptr<Settings> settings_;
private:

	std::string GetCorrectPath(std::string& str);
	void Draw(bool& on_change);

	void SetupStyle();
};
