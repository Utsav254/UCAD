#include "win32/mainWindow.hpp"

mainWindow::mainWindow(const int width, const int height, LPCWSTR windowName, HINSTANCE hInst) :
    window(&wc, hInst, L"MainWindowClass"),
    bt(hInst, 200, 200, 50, 50, 1001),
    _height(height), _width(width),
    _windowName(windowName),
    _hWnd(nullptr)
{}

void mainWindow::createWindow(const bool showWindow, HWND parent)
{
#ifdef _DEBUG
    static unsigned int numCalls = 0;
    numCalls++;

    if (numCalls > 1) __debugbreak();
#endif // _DEBUG

    LPCWSTR className = getClassName();
    HINSTANCE hInst = getInstance();

    _hWnd = CreateWindowExW(
        WS_EX_APPWINDOW,
        className, _windowName,
        WS_OVERLAPPED | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT,
        _width, _height,
        nullptr, nullptr,
        hInst, this);

    if (!_hWnd) throw WINDOW_ERROR;
    if (showWindow) ShowWindow(_hWnd, SW_SHOWNORMAL);
}

LRESULT mainWindow::handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            bt.createWindow(true, hWnd);
            break;
        }
        case WM_CLOSE:
        {
            DestroyWindow(hWnd);
            return 0;
        }
        case WM_DESTROY:
        {
            int i = getInstanceCount();
            if (i == -1) throw ERROR_FMT_M("Internal windowing frame work error ... WNDCLASSEX name not found in _registry of WNDLCASSEX manager");
            if (i == 1) PostQuitMessage(0);
            this->cleanUp();
            return 0;
        }
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

