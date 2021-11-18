#pragma once
#include <vcl/vcl.hpp>

struct gui_parameters {
	bool display_frame = true;
	bool add_sphere = true;
	bool first_person = false;
	int fov;
};


struct CheckboxProp {
	CheckboxProp(std::string& _name, bool& _isChecked, void(*_callback)(bool)) : name(_name), isChecked(_isChecked), callback(_callback) {}
	std::string name;
	bool& isChecked;
	void(*callback)(bool);
};

struct SliderIntProp {
	SliderIntProp(std::string& _name, int& _value, int _value_min, int _value_max, void(*_callback)(int)) 
		: name(_name), value(_value), min_value(_value_min), max_value(_value_max), callback(_callback) {}

	std::string name;
	int& value;
	int min_value;
	int max_value;
	void(*callback)(int);
};

struct SliderFloatProp {
	SliderFloatProp(std::string& _name, float& _value, float _value_min, float _value_max, void(*_callback)(float))
		: name(_name), value(_value), min_value(_value_min), max_value(_value_max), callback(_callback) {}

	std::string name;
	float& value;
	float min_value;
	float max_value;
	void(*callback)(float);
};

template <typename T>
void default_callback(T) {
	return;
}

class GuiWindow {
private :
	std::string gui_name;
	std::vector<CheckboxProp> m_checkboxes;
	std::vector<SliderIntProp> m_slider_ints;
	std::vector<SliderFloatProp> m_slider_floats;


public:
	GuiWindow(const std::string& name);

	bool isFocused();

	void addCheckbox(std::string name, bool& isChecked, void (*callback)(bool) = default_callback);
	void addSliderInt(std::string name, int& value, int min_value, int max_value, void (*callback)(int) = default_callback);
	void addSliderFloat(std::string name, float& value, float min_value, float max_value, void (*callback)(float) = default_callback);

	void draw();
	void display(GLFWwindow* window);
};