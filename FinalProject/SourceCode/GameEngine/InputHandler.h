#pragma once

#include <string>
#include <unordered_map>
#include <bitset>

#include <windows.h>

#include "Input.h"
#include "INIReader.h"

struct CursorPosition {
	long x, y;
};

typedef std::bitset<eIC_Max> InputState;

class InputHandler
{
public:
	InputHandler();

	void Update();

	const std::bitset<eIC_Max>& GetInputState() const;
	const std::bitset<eIC_Max>& GetClickedState() const;
	const CursorPosition GetMouseCoordinates();

private:
	void LoadConfiguration();

	bool IsKeyDown(size_t vk_key);

	void MapSymbol(std::string strSymbol, size_t nSymbol);
	void MapAllSymbols();
	void MapInputEvent(std::size_t nSymbol, size_t nCommand);
	void MapCommandSymbol(std::string strCommand, size_t nCommand, std::string strDefaultSymbol);
	void Remap();

	std::string m_strMapFilePath;

	typedef std::unordered_map<std::string, size_t> TCommandMap;
	typedef std::unordered_map<std::string, size_t> TSymbolMap;
	typedef std::unordered_map<size_t, size_t> TInputEventMap;
	typedef std::unordered_map<std::string, std::string> TCommandSymbolMap;

	TCommandMap m_commandMap;
	TSymbolMap m_symbolMap;
	TInputEventMap m_inputEventMap;
	TCommandSymbolMap m_commandSymbolMap;

	InputState m_InputState;
	InputState m_ClickedState;

	CursorPosition m_cursorPosition;
	HWND m_hwnd;
};

