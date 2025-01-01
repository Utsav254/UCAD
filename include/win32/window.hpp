#pragma once
#include "win32/win.h"
#include <mutex>
#include <unordered_map>
#include <string>
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

class window {
public:

	window(WNDCLASSEXW* wc, HINSTANCE hInst, LPCWSTR className);

	virtual ~window() noexcept;

	static void UnregisterAll() noexcept;

	virtual void destroyWindow() noexcept final;

	virtual LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;

	virtual void createWindow(const bool showWindow, HWND parent) = 0;

public:

	const int getInstanceCount() const noexcept;

	inline const HINSTANCE getInstance() const { return _hInst; }

	inline LPCWSTR getClassName() const { return _className; }

protected:

	HINSTANCE _hInst;
	LPCWSTR _className;

private:

	static inline std::unordered_map<std::wstring, int> _registry;
	static inline std::mutex _mtx;

	static LRESULT CALLBACK handleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	static inline LRESULT CALLBACK handleMessageRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
};