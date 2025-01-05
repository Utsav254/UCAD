#pragma once
#include "util/error.hpp"

#define WINDOW_ERROR windowError(__LINE__ , L"" __FILE__ , L"Window Error", GetLastError())
#define WINDOW_ERROR_HR(hr) windowError(__LINE__ , L"" __FILE__ , L"Window Error", hr)

#ifdef _DEBUG
#define RUN_DX11(callable) if(FAILED(hr = callable)) throw windowError(__LINE__ , L"" __FILE__ , L"Window Error", hr)
#define RUN_DX11_NOTHROW(callable) if(FAILED(callable)) __debugbreak();
#else
#define RUN_DX11(callable) callable
#define RUN_DX11_NOTHROW(callable) callable
#endif
class windowError : public error
{
public:
	windowError(const int line, LPCWSTR fileName, LPCWSTR message, HRESULT hr);
private:
	HRESULT _hr;
};