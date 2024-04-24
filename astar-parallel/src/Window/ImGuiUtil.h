#pragma once

#include <imgui.h>
#include <string>

class OutputMessage
{
public:
	OutputMessage(ImColor normal = ImColor(0.f, 0.8f, 0.2f), ImColor error = ImColor(1.f, 0.1f, 0.1f));

	void setMessage(std::string, bool error = false);
	void clear();

	void draw() const;

private:
	std::string m_message = "";
	bool m_isError = false;
	ImColor m_colorNormal, m_colorError;
};