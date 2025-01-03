#include "MainWindow/mainWindow.hpp"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

mainWindow::mainWindow(const int width, const int height, LPCWSTR windowName, HINSTANCE hInst) :
    window(&wc),
    tb(hInst, 0, 0, width, 200),
    _height(height), _width(width),
    _windowName(windowName),
    _hWnd(nullptr),
    _device(nullptr), _context(nullptr)
{}

void mainWindow::createWindow(const bool showWindow)
{
#ifdef _DEBUG
    static unsigned int numCalls = 0;
    numCalls++;
    if (numCalls > 1) __debugbreak();
#endif // _DEBUG

    _hWnd = CreateWindowExW
    (
        WS_EX_APPWINDOW,
        className, _windowName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        _width, _height,
        nullptr, nullptr,
        _hInst, this
    );

    if (!_hWnd) throw WINDOW_ERROR;

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    UINT createDeviceFlags = 0;

    HRESULT res = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevelArray,
        ARRAYSIZE(featureLevelArray),
        D3D11_SDK_VERSION,
        &_device,
        &featureLevel,
        &_context
    );

    if (res == DXGI_ERROR_UNSUPPORTED) {
        res = D3D11CreateDevice(
            nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags,
            featureLevelArray, ARRAYSIZE(featureLevelArray), D3D11_SDK_VERSION,
            &_device, &featureLevel, &_context
        );
    }
    if (res != S_OK) throw WINDOW_ERROR_HR(res);

    tb.createWindow(true, _hWnd, _device.Get(), _context.Get());
    if (showWindow) ShowWindow(_hWnd, SW_SHOWNORMAL);
}

LRESULT mainWindow::handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_SIZE:
        {
            SetWindowPos(tb.gethWnd(), HWND_TOP, 0, 0, LOWORD(lParam), 200, SWP_NOZORDER | SWP_NOMOVE);
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
            this->destroyWindow();
            return 0;
        }
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

