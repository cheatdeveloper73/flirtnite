#pragma once

#include "includes.h"

const char* const key_names[] = {
	"Unknown",
	"VK_LBUTTON",
	"RBUTTON",
	"CANCEL",
	"MBUTTON",
	"XBUTTON1",
	"XBUTTON2",
	"Unknown",
	"BACK",
	"TAB",
	"Unknown",
	"Unknown",
	"CLEAR",
	"RETURN",
	"Unknown",
	"Unknown",
	"SHIFT",
	"CONTROL",
	"MENU",
	"PAUSE",
	"CAPITAL",
	"KANA",
	"Unknown",
	"JUNJA",
	"FINAL",
	"KANJI",
	"Unknown",
	"ESCAPE",
	"CONVERT",
	"NONCONVERT",
	"ACCEPT",
	"MODECHANGE",
	"SPACE",
	"PRIOR",
	"NEXT",
	"END",
	"HOME",
	"LEFT",
	"UP",
	"RIGHT",
	"DOWN",
	"SELECT",
	"PRINT",
	"EXECUTE",
	"SNAPSHOT",
	"INSERT",
	"DELETE",
	"HELP",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"LWIN",
	"RWIN",
	"APPS",
	"Unknown",
	"SLEEP",
	"NUMPAD0",
	"NUMPAD1",
	"NUMPAD2",
	"NUMPAD3",
	"NUMPAD4",
	"NUMPAD5",
	"NUMPAD6",
	"NUMPAD7",
	"NUMPAD8",
	"NUMPAD9",
	"MULTIPLY",
	"ADD",
	"SEPARATOR",
	"SUBTRACT",
	"DECIMAL",
	"DIVIDE",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"F13",
	"F14",
	"F15",
	"F16",
	"F17",
	"F18",
	"F19",
	"F20",
	"F21",
	"F22",
	"F23",
	"F24",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"NUMLOCK",
	"SCROLL",
	"OEM_NEC_EQUAL",
	"OEM_FJ_MASSHOU",
	"OEM_FJ_TOUROKU",
	"OEM_FJ_LOYA",
	"OEM_FJ_ROYA",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"LSHIFT",
	"RSHIFT",
	"LCONTROL",
	"RCONTROL",
	"LMENU",
	"RMENU"
};

class CKeybind
{
public:

	static enum c_keybind_type : int
	{

		TOGGLE,
		HOLD,
		ALWAYS

	};

	int key = 0;
	c_keybind_type type = TOGGLE;
	const char* name = "none";

	bool enabled = false;
	bool waiting_for_input = false;

	CKeybind(const char* _name)
	{
		this->name = _name;
		this->type = HOLD;
	}

	void update()
	{

		switch (type)
		{

		case TOGGLE:
			if (GetAsyncKeyState(key) & 1)
				enabled = !enabled;
			break;
		case HOLD:
			enabled = GetAsyncKeyState(key);
			break;
		case ALWAYS:
			enabled = true;
			break;

		}

	}

	std::string get_key_name()
	{

		if (!key)
			return "none";

		std::string tmp = key_names[key];
		std::transform(tmp.begin(), tmp.end(), tmp.begin(),
			[](unsigned char c) { return std::tolower(c); });
		return tmp;

	}

	std::string get_name()
	{
		return name;
	}

	std::string get_type()
	{
		switch (type)
		{
		case TOGGLE:
			return "toggle";
			break;
		case HOLD:
			return "hold";
			break;
		case ALWAYS:
			return "always";
			break;
		default:
			return "none";
		}
	}

	bool set_key()
	{

		if (ImGui::IsKeyPressedMap(ImGuiKey_Escape))
		{
			key = 0;
			ImGui::ClearActiveID();
			return true;
		}

		for (auto i = 1; i < 5; i++) {
			if (ImGui::GetIO().MouseDown[i]) {
				switch (i) {
				case 1:
					key = VK_RBUTTON;
					break;
				case 2:
					key = VK_MBUTTON;
					break;
				case 3:
					key = VK_XBUTTON1;
					break;
				case 4:
					key = VK_XBUTTON2;
					break;
				}
				return true;
			}
		}

		for (auto i = VK_BACK; i <= VK_RMENU; i++) {
			if (ImGui::GetIO().KeysDown[i]) {
				key = i;
				return true;
			}
		}

		return false;

	}

};