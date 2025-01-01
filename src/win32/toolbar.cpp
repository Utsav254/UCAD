#include "win32/toolbar.hpp"

toolBar::toolBar(HINSTANCE hInst, int x, int y, int width, int height) :
	window(&wc, hInst, L"ToolbarClass"),
	_x(x), _y(y),
	_width(width), _height(height),
	_hWnd(nullptr), _hWndParent(nullptr)
{}

void toolBar::createWindow(const bool showWindow, HWND parent)
{
	if (parent == nullptr) throw ERROR_FMT_M("toolbar was given nullptr parent handle");
	_hWndParent = parent;
	LPCWSTR className = getClassName();
	HINSTANCE hInst = getInstance();

	_hWnd = CreateWindowExW
	(
		0,
		className,
		L"ToolBar",
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
		_x, _y, _width, _height,
		parent, nullptr,
		hInst, this
	);

	if (!_hWnd) throw WINDOW_ERROR;
	if (showWindow) ShowWindow(_hWnd, SW_SHOWNORMAL);
}

LRESULT toolBar::handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_CLOSE:
		{
			DestroyWindow(hWnd);
			return 0;
		}
		case WM_SIZE:
		{
			_width = LOWORD(lParam);
			MoveWindow(hWnd, _x, _y, _width, _height, TRUE);
			return 0;
		}
		case WM_DESTROY:
		{
			this->destroyWindow();
			return 0;
		}
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

void toolBar::postMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	PostMessageW(_hWnd, msg, wParam, lParam);
	return;
}

LRESULT toolBar::sendMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	return SendMessageW(_hWnd, msg, wParam, lParam);
}
