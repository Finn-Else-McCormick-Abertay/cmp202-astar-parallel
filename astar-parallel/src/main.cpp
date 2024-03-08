#include <GLFW/glfw3.h>
#include <gl/gl.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <extensions/implot/implot.h>

#include "Graph/DirectedGraph.h"
#include "Graph/GraphDisplay.h"

#include <iostream>
#include <vector>

GLFWwindow* g_window = nullptr;
int g_windowWidth = 640, g_windowHeight = 480;
DirectedGraph<float, int>* g_graph = nullptr;

void imguiUpdate() {
	ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(g_windowWidth, g_windowHeight), ImGuiCond_Always);
	ImGui::Begin("Window", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::Text("Some text");

	if (g_graph) {
		DisplayGraph<float,int>(*g_graph,
			[](float val, int index){
				return ImVec2(val, index);
			}
		);
	}

	ImGui::End();
}

void render() {
	ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();
	glfwSwapBuffers(g_window); glClear(GL_COLOR_BUFFER_BIT);
	imguiUpdate();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void onResized(GLFWwindow* window, int width, int height) {
	g_windowWidth = width; g_windowHeight = height;
	render();
}

int main() {
	if (!glfwInit()) {
		std::cout << "GLFW failed to initialise." << std::endl;
		return 0;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	g_window = glfwCreateWindow(g_windowWidth, g_windowHeight, "My Title", NULL, NULL);
	glfwMakeContextCurrent(g_window);
	//glfwSwapInterval(1); // Vsync

	glfwSetWindowSizeCallback(g_window, onResized);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext(); ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(g_window, true);
	ImGui_ImplOpenGL3_Init();

	glClearColor(1.f, 1.f, 1.f, 1.f);

	DirectedGraph<float, int> graph(
		{ 1, 5, 6, 3, 4, 8 },
		{ {0,1},{1,5},{5,4},{4,1},{4,2},{2,3},{2,0},{2,5} }
	);
	g_graph = &graph;

	while (!glfwWindowShouldClose(g_window)) {
		glfwWaitEvents();
		render();
	}

	ImGui_ImplOpenGL3_Shutdown(); ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext(); ImPlot::DestroyContext();

	glfwTerminate();
	return 0;
}