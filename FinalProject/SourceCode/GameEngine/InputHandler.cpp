#include "InputHandler.h"

#include <windows.h>
#include <algorithm>
#include <cassert>

InputHandler::InputHandler()
{
	m_strMapFilePath = "../../../Assets/Configs/actionmap.ini";
	std::replace(m_strMapFilePath.begin(), m_strMapFilePath.end(), '\\', '/');

	MapAllSymbols();

	MapCommandSymbol("GoLeft", eIC_GoLeft, "a");
	MapCommandSymbol("GoRight", eIC_GoRight, "d");
	MapCommandSymbol("GoForward", eIC_GoForward, "w");
	MapCommandSymbol("GoBackward", eIC_GoBackward, "s");
	MapCommandSymbol("Jump", eIC_Jump, "space");
	MapCommandSymbol("Shoot", eIC_Shoot, "LMB");

	LoadConfiguration();

	Remap();

	m_hwnd = GetActiveWindow();
}

bool InputHandler::IsKeyDown(size_t vk_key)
{
	if (GetAsyncKeyState(vk_key) & 0x8000)
		return true;

	return false;
}

void InputHandler::MapSymbol(std::string strSymbol, size_t nSymbol)
{
	m_symbolMap[strSymbol] = nSymbol;
}

void InputHandler::MapInputEvent(std::size_t nSymbol, size_t nCommand)
{
	m_inputEventMap[nSymbol] = nCommand;
}

void InputHandler::MapCommandSymbol(std::string strCommand, size_t nCommand, std::string strDefaultSymbol)
{
	m_commandMap[strCommand] = nCommand;
	m_commandSymbolMap[strCommand] = strDefaultSymbol;
}

void InputHandler::Remap()
{
	for (auto& it : m_commandSymbolMap)
	{
		MapInputEvent(m_symbolMap[it.second], m_commandMap[it.first]);
	}
}

// We used int as return type just for demonstration. It should be done another way
void InputHandler::Update()
{
	for (auto& it : m_inputEventMap)
	{
		m_InputState.set(it.second, IsKeyDown(it.first));
	}
}

const std::bitset<eIC_Max>& InputHandler::GetInputState() const
{
	return m_InputState;
}

const CursorPosition InputHandler::GetMouseCoordinates() {
	GetCursorPos((LPPOINT)(&m_cursorPosition));
	ScreenToClient(m_hwnd, (LPPOINT)(&m_cursorPosition));
	return m_cursorPosition;
}


void InputHandler::LoadConfiguration()
{
	INIReader reader(m_strMapFilePath.c_str());
	assert(reader.ParseError() >= 0);
	auto fields = reader.GetFields("Keyboard");

	for (auto& field : fields)
	{
		std::string strCommand = field;
		std::string strSymbol = reader.Get("Keyboard", field, "");

		m_commandSymbolMap[strCommand] = strSymbol;
	}
}