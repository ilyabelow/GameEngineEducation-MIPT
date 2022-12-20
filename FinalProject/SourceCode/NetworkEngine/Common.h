#pragma once

#pragma warning(disable:4251)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

#ifdef NETWORKENGINE_EXPORTS
#define NETWORKENGINE_API __declspec(dllexport)
#else
#define NETWORKENGINE_API __declspec(dllimport)
#endif