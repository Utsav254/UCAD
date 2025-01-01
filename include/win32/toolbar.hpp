#pragma once
#include "win32/window.hpp"

class toolBar : window
{
public:
	toolBar(HINSTANCE hInst,
		int x, int y,
		int width, int height
	);

	~toolBar() = default;

	void createWindow(const bool showWindow, HWND parent) override;

	LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	void postMessage(UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT sendMessage(UINT msg, WPARAM wParam, LPARAM lParam);

private:

	int _x, _y;
	int _width, _height;

	HWND _hWnd, _hWndParent;

private:
	static inline WNDCLASSEX wc =
	{
		.cbSize = sizeof(WNDCLASSEXW),
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = DefWindowProcW,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = nullptr,
		.hIcon = nullptr,
		.hCursor = nullptr,
		.hbrBackground = nullptr,
		.lpszMenuName = nullptr,
		.lpszClassName = nullptr,
		.hIconSm = nullptr,
	};
};

