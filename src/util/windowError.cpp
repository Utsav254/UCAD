#include "util/windowError.hpp"

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