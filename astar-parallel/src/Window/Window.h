#pragma once

#include <GLFW/glfw3.h>
#include <gl/gl.h>

#include <functional>

class Window
{
public:
	~Window();

	static bool init(std::function<void(int, int)> imguiUpdate, std::function<float()> imguiTitlebar, int width = 640, int height = 480);

	static void enterLoop();

	static void update();
	static void render();
	static void onResized(GLFWwindow* window, int width, int height);

private:
	static Window& singleton();

	GLFWwindow* m_window;
	int m_windowWidth, m_windowHeight;
	std::function<void(int,int)> m_imguiUpdate;
	std::function<float()> m_imguiTitlebar;
};
