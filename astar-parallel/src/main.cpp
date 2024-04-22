#include "Graph/DirectedGraph.h"
#include "Graph/GraphDisplay.h"

#include <vector>

#include "Window/Window.h"
#include "Window/GraphEdit.h"
#include "Window/PathfindingSettings.h"

#include "Pathfinding/AStar.h"
#include <iostream>

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

	DisplayGraph<Vec2, float>(Singleton::graph(), Singleton::path(),
		[](const Vec2& val, const int& index){
			NodeDisplayInfo info;
			info.name = std::to_string(index);
			info.pos.x = val.x; info.pos.y = val.y;
			return info;
		}
	);
}

int main() {
	Singleton::graph() = DirectedGraph<Vec2, float>(
		{ {0,0}, {-1,-2}, {1,-2}, {-1,-4}, {1,-4}, {0,-6} },
		{ {0,1},{1,5},{5,4},{4,1},{4,2},{2,3},{2,0},{2,5} },
		true
	);
	Singleton::recalculateEdgeWeights();

	if (Window::init(imguiUpdate, imguiTitlebar, 1200, 800)) {
		Window::enterLoop();
	}
}
