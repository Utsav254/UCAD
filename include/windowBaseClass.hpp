#pragma once
#include <win.h>
#include <atomic>
#include "util/error.hpp"

#define WINDOW_ERROR windowError(__LINE__ , L"" __FILE__ , L"Window Error", GetLastError())
#define WINDOW_ERROR_HR(hr) windowError(__LINE__ , L"" __FILE__ , L"Window Error", hr)

class windowError : public error
{
public:
	windowError(const int line, LPCWSTR fileName, LPCWSTR message, HRESULT hr) :
		error(line, fileName, message), _hr(hr)
	{
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

private:
	HRESULT _hr;
};

class window {
public:

	virtual inline const HINSTANCE getInstance() const { return GetModuleHandleW(nullptr); }
	virtual LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;

	window(LPCWSTR className, WNDCLASSEXW* wc) : _className(className)
	{
		if (!_isRegistered) {
			wc->lpfnWndProc = handleMessageSetup;
			wc->lpszClassName = className;
			wc->hInstance = getInstance();
			if (RegisterClassExW(wc) == 0) throw WINDOW_ERROR;
			_isRegistered = true;
		}
		_instanceCount++;
	}

	virtual ~window() noexcept
	{
		_instanceCount--;
		if (_instanceCount.load() <= 0) {
			UnregisterClassW(_className, getInstance());
		}
	}

protected:
	static inline std::atomic<int> _instanceCount{ 0 };
	static inline bool _isRegistered = false;
	const LPCWSTR _className;

private:
	static LRESULT CALLBACK handleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		if (msg == WM_NCCREATE) {
			const CREATESTRUCTW* const tempCreateStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
			window* const tempWindow = static_cast<window*>(tempCreateStruct->lpCreateParams);

			SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(tempWindow));
			SetWindowLongPtrW(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&window::handleMessageRedirect));
			return tempWindow->handleMessage(hWnd, msg, wParam, lParam);
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	static inline LRESULT CALLBACK handleMessageRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
		window* const tempWindow = reinterpret_cast<window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		return tempWindow->handleMessage(hWnd, msg, wParam, lParam);
	}
};