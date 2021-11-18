#include "GuiWindow.hpp"

GuiWindow::GuiWindow(const std::string& name) : gui_name(name) {
	
}

bool GuiWindow::isFocused() {
	return ImGui::IsAnyWindowFocused();
}

void GuiWindow::addCheckbox(std::string name, bool& isChecked, void(*callback)(bool)) {
	m_checkboxes.emplace_back(CheckboxProp(name, isChecked, callback));
}

void GuiWindow::addSliderInt(std::string name, int& value, int min_value, int max_value, void(*callback)(int)) {
	m_slider_ints.emplace_back(SliderIntProp(name, value, min_value, max_value, callback));
}

void GuiWindow::addSliderFloat(std::string name, float& value, float min_value, float max_value, void (*callback)(float)) {
	m_slider_floats.emplace_back(SliderFloatProp(name, value, min_value, max_value, callback));
}


void GuiWindow::draw() {
	vcl::imgui_create_frame();
	ImGui::Begin("GUI", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	for (auto& chck : m_checkboxes) {
		ImGui::Checkbox(chck.name.c_str(), &chck.isChecked);
		chck.callback(chck.isChecked);
	}
	for (auto& slid : m_slider_ints) {
		// If slider is being changed. Callback function makes us save some good performance.
		if (ImGui::SliderInt(slid.name.c_str(), &slid.value, slid.min_value, slid.max_value)) {
			slid.callback(slid.value);
		}
	}
	for (auto& slid : m_slider_floats) {
		// If slider is being changed. Callback function makes us save some good performance.
		if (ImGui::SliderFloat(slid.name.c_str(), &slid.value, slid.min_value, slid.max_value)) {
			slid.callback(slid.value);
		}
	}
	ImGui::End();
}

void GuiWindow::display(GLFWwindow* window) {
	vcl::imgui_render_frame(window);
}
