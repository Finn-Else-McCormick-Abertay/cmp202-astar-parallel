#include "Window.h"

#include <imgui.h>
#include <extensions/implot/implot.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>

Window& Window::singleton() {
	static std::unique_ptr<Window> ptr = std::make_unique<Window>();
	return *ptr;
}

bool Window::init(std::function<void()> imguiUpdate, int width, int height) {
	if (singleton().m_window != nullptr) { return false; }

	if (!glfwInit()) {
		std::cout << "GLFW failed to initialise." << std::endl;
		return false;
	}

	singleton().m_windowWidth = width;
	singleton().m_windowHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	singleton().m_window = glfwCreateWindow(singleton().m_windowWidth, singleton().m_windowHeight, "CMP202 CPU Multithreading Project", NULL, NULL);
	glfwMakeContextCurrent(singleton().m_window);
	glfwSwapInterval(1); // Vsync

	glfwSetWindowSizeCallback(singleton().m_window, onResized);
	singleton().m_imguiUpdate = std::function(imguiUpdate);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext(); ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(singleton().m_window, true);
	ImGui_ImplOpenGL3_Init();

	auto& style = ImGui::GetStyle();
	ImGui::StyleColorsLight(&style);
	style.WindowBorderSize = 0.f;
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.f, 1.f, 1.f, 1.f);

	return true;
}

Window::~Window() {
	ImGui_ImplOpenGL3_Shutdown(); ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext(); ImPlot::DestroyContext();

	glfwTerminate();
}

void Window::render() {
	ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();
	update();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(singleton().m_window);
}

void Window::update() {
	float menuBarHeight = 0.f;
	if (ImGui::BeginMainMenuBar()) {
		menuBarHeight = ImGui::GetWindowHeight();
		//if (ImGui::BeginMenu("File")) {
		//	ImGui::EndMenu();
		//}
		ImGui::EndMainMenuBar();
	}

	ImGui::SetNextWindowPos({ 0, menuBarHeight }, ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(singleton().m_windowWidth, singleton().m_windowHeight - menuBarHeight), ImGuiCond_Always);
	ImGui::Begin("MainWindow", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration);

	singleton().m_imguiUpdate();

	ImGui::End();
}

void Window::onResized(GLFWwindow* window, int width, int height) {
	singleton().m_windowWidth = width; singleton().m_windowHeight = height;
	render();
}

void Window::enterLoop() {
	while (!glfwWindowShouldClose(singleton().m_window)) {
		glfwWaitEvents();
		render();
	}
}