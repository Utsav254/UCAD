#pragma once
#include "util/error.hpp"

#define WINDOW_ERROR windowError(__LINE__ , L"" __FILE__ , L"Window Error", GetLastError())
#define WINDOW_ERROR_HR(hr) windowError(__LINE__ , L"" __FILE__ , L"Window Error", hr)

class windowError : public error
{
public:
	windowError(const int line, LPCWSTR fileName, LPCWSTR message, HRESULT hr);
private:
	HRESULT _hr;
};