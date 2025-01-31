#include "pch.hpp"
#include "error.hpp"
#include <format>

error::error(const int line , LPCWSTR fileName, LPCWSTR message) noexcept :
	_line(line), _fileName(fileName)
{
    if (message == nullptr) _whatBuffer = L"";
    else _whatBuffer = std::move(std::wstring(message));
}

int error::display() const noexcept
{
    try {
        std::wstring errMsg = std::format(L"File: {}:{}\n{}", _fileName, _line, _whatBuffer);
        return MessageBoxExW(nullptr, errMsg.c_str(), L"Error", MB_OK | MB_ICONEXCLAMATION, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
    }
    catch (std::exception&) {
        return MessageBoxExW(nullptr, L"Failed to display error message.", L"Error", MB_OK | MB_ICONERROR, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
    }
}

const std::wstring& error::what() const noexcept
{
	return _whatBuffer;
}

const int error::getLine() const noexcept
{
	return _line;
}

LPCWSTR error::getFile() const noexcept
{
	return _fileName;
}

windowError::windowError(const int line, LPCWSTR fileName, LPCWSTR message, HRESULT hr) :
	error(line, fileName, message), _hr(hr)
{
	_whatBuffer += L"\n";
	LPWSTR errorMsg = nullptr;
	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
		(LPWSTR)&errorMsg, 0, nullptr
	);
	_whatBuffer += errorMsg ? std::wstring(errorMsg) : L"Unknown error";
	if (errorMsg) LocalFree(errorMsg);
}