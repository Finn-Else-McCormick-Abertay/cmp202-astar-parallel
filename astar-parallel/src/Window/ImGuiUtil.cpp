#include "ImGuiUtil.h"

OutputMessage::OutputMessage(ImColor normal, ImColor error) : m_colorNormal(normal), m_colorError(error) {}

void OutputMessage::setMessage(std::string msg, bool error) {
	m_message = msg; m_isError = error;
}

void OutputMessage::clear() {
	m_message = ""; m_isError = false;
}

void OutputMessage::draw() const { ImGui::TextColored(m_isError ? m_colorError : m_colorNormal, " %s", m_message.c_str()); }