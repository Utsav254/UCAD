#pragma once
#include "windowBaseClass.hpp"

class mainWindow : public window<mainWindow>
{
public:

    mainWindow(const int width, const int height, LPCWSTR windowName, HINSTANCE hInst) :
        window(&wc, hInst),
        _height(height), _width(width), _windowName(windowName),
        _hWnd(
            CreateWindowExW(
                WS_EX_APPWINDOW,
                getClassName(), nullptr,
                WS_OVERLAPPEDWINDOW & ~(WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX),
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

    LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
            RECT rect;
            GetClientRect(hWnd, &rect);
            FillRect(hdc, &rect, hBrush);
            DeleteObject(hBrush);
            EndPaint(hWnd, &ps);
            return 0;
        }
        default:
            return DefWindowProcW(hWnd, msg, wParam, lParam);
        }
    }

    static inline constexpr LPCWSTR getClassName() { return L"MainWindowClass"; }

    inline const HINSTANCE getInstance() const { return _hInst; }

    inline const HWND getHandle() const { return _hWnd; }

private:
    int _height, _width;
    LPCWSTR _windowName;
    const HWND _hWnd;

private:
    static inline WNDCLASSEX wc =
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
        .lpszClassName = getClassName(),
        .hIconSm = nullptr,
    };
};
