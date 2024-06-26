#include "Graph/DirectedGraph.h"
#include "Graph/GraphDisplay.h"

#include "Window/Window.h"
#include "Window/GraphEdit.h"
#include "Window/PathfindingSettings.h"

#include "Pathfinding/AStar.h"
#include "Maths/Vec2.h"

#include "Singleton.h"

static GraphEdit g_graphEdit;
static PathfindingSettings g_pathfindingSettings;

float imguiTitlebar() {
	float menuBarHeight = 0.f;
	if (ImGui::BeginMainMenuBar()) {
		menuBarHeight = ImGui::GetWindowHeight();
		g_graphEdit.addMenuBarItem();
		g_pathfindingSettings.addMenuBarItem();
		ImGui::EndMainMenuBar();
	}
	return menuBarHeight;
}

void imguiUpdate(int width, int height) {
	g_graphEdit.imguiDrawWindow(width, height);
	g_pathfindingSettings.imguiDrawWindow(width, height);

	g_pathfindingSettings.imguiDrawControlGroup();
	g_graphEdit.imguiDisplayGraph();

	auto [pathStart, pathEnd] = g_pathfindingSettings.getIndices();
	DisplayGraph<Vec2, float>(Singleton::graph(), Singleton::path(), { pathStart, pathEnd }, GetNodeInfoFunc<Vec2>(GraphEdit::getNodeInfo));
}

int main() {
	if (Window::init(imguiUpdate, imguiTitlebar, 1200, 800)) {
		g_graphEdit.generateGraph();
		Window::enterLoop();
	}
}	
