#include "InputHandler.h"

#include <windows.h>

void InputHandler::MapAllSymbols() {
	MapSymbol("LMB", VK_LBUTTON);
	MapSymbol("RMB", VK_RBUTTON);
	MapSymbol("MMB", VK_MBUTTON);

	MapSymbol("backspace", VK_BACK);
	MapSymbol("tab", VK_TAB);
	MapSymbol("clear", VK_CLEAR);
	MapSymbol("enter", VK_RETURN);

	MapSymbol("shift", VK_SHIFT);
	MapSymbol("control", VK_CONTROL);
	MapSymbol("alt", VK_MENU);
	MapSymbol("pause", VK_PAUSE);
	MapSymbol("capslock", VK_CAPITAL);
	MapSymbol("esc", VK_ESCAPE);
	MapSymbol("space", VK_SPACE);

	MapSymbol("pgup", VK_PRIOR);
	MapSymbol("pgdn", VK_NEXT);
	MapSymbol("end", VK_END);
	MapSymbol("home", VK_HOME);

	MapSymbol("left", VK_LEFT);
	MapSymbol("right", VK_RIGHT);
	MapSymbol("up", VK_UP);
	MapSymbol("down", VK_DOWN);

	MapSymbol("prtsc", VK_SNAPSHOT);
	MapSymbol("insert", VK_INSERT);
	MapSymbol("delete", VK_DELETE);

	MapSymbol("lwin", VK_LWIN);
	MapSymbol("rwin", VK_RWIN);

	MapSymbol("numpad0", VK_NUMPAD0);
	MapSymbol("numpad1", VK_NUMPAD1);
	MapSymbol("numpad2", VK_NUMPAD2);
	MapSymbol("numpad3", VK_NUMPAD3);
	MapSymbol("numpad4", VK_NUMPAD4);
	MapSymbol("numpad5", VK_NUMPAD5);
	MapSymbol("numpad6", VK_NUMPAD6);
	MapSymbol("numpad7", VK_NUMPAD7);
	MapSymbol("numpad8", VK_NUMPAD8);
	MapSymbol("numpad9", VK_NUMPAD9);

	MapSymbol("multiply", VK_MULTIPLY);
	MapSymbol("add", VK_ADD);
	MapSymbol("subtract", VK_SUBTRACT);
	MapSymbol("decimal", VK_DECIMAL);
	MapSymbol("divide", VK_DIVIDE);

	MapSymbol("f1", VK_F1);
	MapSymbol("f2", VK_F2);
	MapSymbol("f3", VK_F3);
	MapSymbol("f4", VK_F4);
	MapSymbol("f5", VK_F5);
	MapSymbol("f6", VK_F6);
	MapSymbol("f7", VK_F7);
	MapSymbol("f8", VK_F8);
	MapSymbol("f9", VK_F9);
	MapSymbol("f10", VK_F10);
	MapSymbol("f11", VK_F11);
	MapSymbol("f12", VK_F12);
	MapSymbol("f13", VK_F13);
	MapSymbol("f14", VK_F14);
	MapSymbol("f15", VK_F15);
	MapSymbol("f16", VK_F16);
	MapSymbol("f17", VK_F17);
	MapSymbol("f18", VK_F18);
	MapSymbol("f19", VK_F19);
	MapSymbol("f20", VK_F20);
	MapSymbol("f21", VK_F21);
	MapSymbol("f22", VK_F22);
	MapSymbol("f23", VK_F23);
	MapSymbol("f24", VK_F24);

	MapSymbol("numlock", VK_NUMLOCK);
	MapSymbol("scrolllock", VK_SCROLL);

	MapSymbol("lshift", VK_LSHIFT);
	MapSymbol("rshift", VK_RSHIFT);
	MapSymbol("lcontrol", VK_LCONTROL);
	MapSymbol("rcontrol", VK_RCONTROL);
	MapSymbol("lalt", VK_LMENU);
	MapSymbol("ralt", VK_RMENU);
	MapSymbol(";", VK_OEM_1);
	MapSymbol("+", VK_OEM_PLUS);
	MapSymbol(",", VK_OEM_COMMA);
	MapSymbol("-", VK_OEM_MINUS);
	MapSymbol(".", VK_OEM_PERIOD);
	MapSymbol("/", VK_OEM_2);
	MapSymbol("~", VK_OEM_3);
	MapSymbol("[", VK_OEM_4);
	MapSymbol("\\", VK_OEM_5);
	MapSymbol("]", VK_OEM_6);
	MapSymbol("\'", VK_OEM_7);

	MapSymbol("volume_mute", VK_VOLUME_MUTE);
	MapSymbol("volume_down", VK_VOLUME_DOWN);
	MapSymbol("volume_up", VK_VOLUME_UP);

	MapSymbol("a", 'A');
	MapSymbol("b", 'B');
	MapSymbol("c", 'C');
	MapSymbol("d", 'D');
	MapSymbol("e", 'E');
	MapSymbol("f", 'F');
	MapSymbol("g", 'G');
	MapSymbol("h", 'H');
	MapSymbol("i", 'I');
	MapSymbol("j", 'J');
	MapSymbol("k", 'K');
	MapSymbol("l", 'L');
	MapSymbol("m", 'M');
	MapSymbol("n", 'N');
	MapSymbol("o", 'O');
	MapSymbol("p", 'P');
	MapSymbol("q", 'Q');
	MapSymbol("r", 'R');
	MapSymbol("s", 'S');
	MapSymbol("t", 'T');
	MapSymbol("u", 'U');
	MapSymbol("v", 'V');
	MapSymbol("w", 'W');
	MapSymbol("x", 'X');
	MapSymbol("y", 'Y');
	MapSymbol("z", 'Z');
}