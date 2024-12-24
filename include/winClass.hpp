#pragma once
#include "windowBaseClass.hpp"

class mainWindow : public window<mainWindow>
{
public:
    static constexpr LPCWSTR className = L"MainWindow";

	mainWindow(const int width, const int height, LPCWSTR windowName):
		_height(height), _width(width), _windowName(windowName),
        _hWnd(
            CreateWindowExW(
                0,
                className, nullptr,
                WS_SYSMENU,
                CW_USEDEFAULT, CW_USEDEFAULT,
                width, height,
                nullptr, nullptr,
                getInstance(), nullptr)
            )
	{
        ShowWindow(_hWnd, SW_SHOWNORMAL);
    }

	~mainWindow() noexcept = default;

	static WNDCLASSEXW* defineWindowClass() noexcept
    {
        static WNDCLASSEX wc = { 0 };
        wc.cbSize = sizeof(wc);
        wc.style = CS_OWNDC | CS_DBLCLKS;
        wc.lpfnWndProc = DefWindowProcW;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = getInstance();
        wc.hIcon = nullptr;
        wc.hCursor = nullptr;
        wc.hbrBackground = nullptr;
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = className;
        wc.hIcon = nullptr;
        return &wc;
    }

    LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_CLOSE:
            if (this->_instanceCount.load() == 1) PostQuitMessage(0);
            else DestroyWindow(hWnd);
            return 0;
        case WM_DESTROY:
            std::destroy_at(this);
            return 0;
        default:
            return DefWindowProcW(hWnd, msg, wParam, lParam);
        }
    }

    static inline const HINSTANCE getInstance()
    {
        static const HINSTANCE hInst = GetModuleHandleW(nullptr);
        return hInst;
    }

    inline const HWND getHandle() const { return _hWnd; }

private:
	int _height, _width;
	LPCWSTR _windowName;
    const HWND _hWnd;
};
