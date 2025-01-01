#pragma once
#include "win32/window.hpp"
#include "win32/toolbar.hpp"

class mainWindow : public window {
public:
    mainWindow(const int width, const int height, LPCWSTR windowName, HINSTANCE hInst);

    ~mainWindow() = default;

    void createWindow(const bool showWindow, HWND parent) override;

    LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

public:
    inline const HWND getWindowHandle() const { return _hWnd; }

private:

    toolBar tb;

    int _height, _width;
    LPCWSTR _windowName;
    HWND _hWnd;

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
        .hCursor = LoadCursorW(nullptr, IDC_ARROW),
        .hbrBackground = nullptr,
        .lpszMenuName = nullptr,
        .lpszClassName = nullptr,
        .hIconSm = nullptr,
    };
};
