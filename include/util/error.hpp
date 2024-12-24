#pragma once
#include <string>
#include "win.h"

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

#define ERROR_FMT error(__LINE__ , L"" __FILE__ , nullptr)
#define ERROR_FMT_M(message) error(__LINE__ , L"" __FILE__ , L"" message)