#pragma once
#include "util/win.h"
#include "util/windowError.hpp"
#include <mutex>
#include <unordered_map>
#include <string>

class window {
public:

	window(WNDCLASSEXW* wc);
	virtual ~window() noexcept;

	static void startUp(HINSTANCE hInst) noexcept;
	static void shutDown() noexcept;

	virtual void destroyWindow() noexcept final;

	virtual LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;

public:
	const int getInstanceCount() const noexcept;

	inline LPCWSTR getClassName() const { return _className; }

protected:
	static inline HINSTANCE _hInst = nullptr;

	LPCWSTR _className;

private:
	static inline std::unordered_map<std::wstring, int> _registry;
	static inline std::mutex _mtx;

	static LRESULT CALLBACK handleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static inline LRESULT CALLBACK handleMessageRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
};