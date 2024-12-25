#pragma once
#include "windowBaseClass.hpp"

class mainWindow : public window {
public:
	static constexpr LPCWSTR className = L"MainWindow";

	mainWindow(const int width, const int height, LPCWSTR windowName, HINSTANCE hInst) :
		window(className, &wc),
		_height(height), _width(width), _windowName(windowName),
		_hInst(hInst),
		_hWnd(
			CreateWindowExW(
				0,
				className, nullptr,
				WS_POPUP | WS_BORDER,
				CW_USEDEFAULT, CW_USEDEFAULT,
				width, height,
				nullptr, nullptr,
				getInstance(), this)
			)
	{
		if (!_hWnd) throw WINDOW_ERROR;
		ShowWindow(_hWnd, SW_SHOWNORMAL);
	}

	~mainWindow() noexcept = default;

	LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override
	{
		switch (msg)
		{
		case WM_CLOSE:
			DestroyWindow(hWnd);
			return 0;
		case WM_DESTROY:
			if (this->_instanceCount.load() == 1) PostQuitMessage(0);
			this->_instanceCount--;
			return 0;
		case WM_NCHITTEST:
		{
			POINT pt = { LOWORD(lParam),HIWORD(lParam) };
			ScreenToClient(hWnd, &pt);

			if (pt.y < (int)(0.05 * _height)) return HTCAPTION;
			else return HTCLIENT;
		}
		default:
			return DefWindowProcW(hWnd, msg, wParam, lParam);
		}
	}

	inline const HINSTANCE getInstance() const override { return _hInst; }
	inline const HWND getHandle() const { return _hWnd; }

private:
	int _height, _width;
	LPCWSTR _windowName;
	const HINSTANCE _hInst;
	const HWND _hWnd;

	static inline WNDCLASSEXW wc =
	{
		.cbSize = sizeof(WNDCLASSEXW),
		.style = CS_OWNDC | CS_DBLCLKS,
		.lpfnWndProc = DefWindowProcW,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = nullptr,
		.hIcon = nullptr,
		.hCursor = nullptr,
		.hbrBackground = nullptr,
		.lpszMenuName = nullptr,
		.lpszClassName = nullptr,
		.hIconSm = nullptr
	};
};
