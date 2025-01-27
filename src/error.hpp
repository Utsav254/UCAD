#pragma once
#include "win.h"
#include <string>

#define ERROR_FMT error(__LINE__ , L"" __FILE__ , nullptr)
#define ERROR_FMT_M(message) error(__LINE__ , L"" __FILE__ , L"" message)


#define WINDOW_ERROR windowError(__LINE__ , L"" __FILE__ , L"Window Error", GetLastError())
#define WINDOW_ERROR_HR(hr) windowError(__LINE__ , L"" __FILE__ , L"Window Error", hr)

#ifdef _DEBUG
#define RUN_DX11(callable) if(FAILED(hr = callable)) throw windowError(__LINE__ , L"" __FILE__ , L"Window Error", hr)
#else
#define RUN_DX11(callable) callable
#endif

class error
{
public:

	error(const int line, LPCWSTR fileName, LPCWSTR message) noexcept;
	virtual ~error() = default;
	
	virtual int display() const noexcept;
	const std::wstring& what() const noexcept;

	const int getLine() const noexcept;
	LPCWSTR getFile() const noexcept;

protected:
	std::wstring _whatBuffer;
	const int _line;
	LPCWSTR _fileName;
};

class windowError : public error
{
public:
	windowError(const int line, LPCWSTR fileName, LPCWSTR message, HRESULT hr);
private:
	HRESULT _hr;
};