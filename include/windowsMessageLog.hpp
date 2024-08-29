#pragma once
#include <UCADWin.h>
#include <string>
#include <unordered_map>

#define GET_MESSAGE(msg) { msg , #msg }

class windowsMessageLog {
public:
	windowsMessageLog() = default;
	~windowsMessageLog() = default;

	void get_str(std::wstring& in, const UINT msg, const WPARAM wParam, const LPARAM lParam) const ;

private:
	static const std::unordered_map<UINT, const char *> map;
};