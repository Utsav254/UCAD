#include "MainWindow/mainWindow.hpp"

mainWindow::mainWindow(const int width, const int height, const LPCWSTR windowName) :
    window(&wc),
    tb(0, 0, width, 200),
    edt(0, 200, width, height - 200),
    _height(height), _width(width),
    _windowName(windowName),
    _hWnd(nullptr),
    _device(nullptr), _context(nullptr)
{}

void mainWindow::createWindow(const bool showWindow, HWND parent, ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
#ifdef _DEBUG
    static unsigned int numCalls = 0;
    numCalls++;
    if (numCalls > 1) throw ERROR_FMT_M("Main Window error ... attempting to call createWindow twice on same instance");

    if (parent != nullptr) throw ERROR_FMT_M("Main window given non nullptr parent window handle");
#else
    (void)parent;
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
#ifdef _DEBUG
    UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#else
    UINT createDeviceFlags = 0;
#endif

    if (device == nullptr && context == nullptr) {
        HRESULT hr = D3D11CreateDevice(
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
        if (hr == DXGI_ERROR_UNSUPPORTED) {
            hr = D3D11CreateDevice(
                nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags,
                featureLevelArray, ARRAYSIZE(featureLevelArray), D3D11_SDK_VERSION,
                &_device, &featureLevel, &_context
            );
        }
        if (hr != S_OK) throw WINDOW_ERROR_HR(hr);
    }
    else if (device == nullptr && context != nullptr) throw ERROR_FMT_M("Main window given null device bu non nullptr context");
    else if (device != nullptr && context == nullptr) {
        _device = device;
        _device->GetImmediateContext(&_context);
    }
    else {
        _device = device;
        _context = context;
    }

    try {
        tb.createWindow(true, _hWnd, _device, _context);
        edt.createWindow(true, _hWnd, _device, _context);
    }
    catch (const error& e) {
        e.display();
    }

    if (showWindow) ShowWindow(_hWnd, SW_SHOWNORMAL);
    UpdateWindow(_hWnd);
}

LRESULT mainWindow::handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            RECT size_rect;
            GetWindowRect(hWnd, &size_rect);
            SetWindowPos(
                hWnd, NULL,
                size_rect.left, size_rect.top,
                size_rect.right - size_rect.left, size_rect.bottom - size_rect.top,
                SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE
            );
            break;
        }
        case WM_NCCALCSIZE: {
            if (!wParam) break;
            UINT dpi = GetDpiForWindow(hWnd);

            int frame_x = GetSystemMetricsForDpi(SM_CXFRAME, dpi);
            int frame_y = GetSystemMetricsForDpi(SM_CYFRAME, dpi);
            int padding = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);

            NCCALCSIZE_PARAMS* params = (NCCALCSIZE_PARAMS*)lParam;
            RECT* requested_client_rect = params->rgrc;

            requested_client_rect->right -= frame_x + padding;
            requested_client_rect->left += frame_x + padding;
            requested_client_rect->bottom -= frame_y + padding;

            WINDOWPLACEMENT placement = { 0 };
            placement.length = sizeof(WINDOWPLACEMENT);
            if (GetWindowPlacement(hWnd, &placement)) {
                if (placement.showCmd == SW_SHOWMAXIMIZED) {
                    requested_client_rect->top += padding;

                }
            }
            return 0;
        }
        case WM_NCHITTEST:
        {
            if (LOWORD(wParam)) {
                return HTCAPTION;
            }
            POINT ptMouse = { LOWORD(lParam), HIWORD(lParam) };
            RECT rectWindow;
            GetWindowRect(hWnd, &rectWindow);
            int borderWidth = 8;
            int titleBarHeight = 30;
            int buttonWidth = 50;
            int buttonHeight = titleBarHeight;
            LONG windowWidth = rectWindow.right - rectWindow.left;
            LONG windowHeight = rectWindow.bottom - rectWindow.top;

            int mouseX = ptMouse.x - rectWindow.left;
            int mouseY = ptMouse.y - rectWindow.top;

            if (mouseX < borderWidth && mouseY < borderWidth)
                return HTTOPLEFT;
            if (mouseX > windowWidth - borderWidth && mouseY < borderWidth)
                return HTTOPRIGHT;
            if (mouseX < borderWidth && mouseY > windowHeight - borderWidth)
                return HTBOTTOMLEFT;
            if (mouseX > windowWidth - borderWidth && mouseY > windowHeight - borderWidth)
                return HTBOTTOMRIGHT;

            if (mouseY < borderWidth)
                return HTTOP;
            if (mouseY > windowHeight - borderWidth)
                return HTBOTTOM;
            if (mouseX < borderWidth)
                return HTLEFT;
            if (mouseX > windowWidth - borderWidth)
                return HTRIGHT;

            if (mouseY < 50) {
                return HTCAPTION;
            }

            return HTCLIENT;
        }
        case WM_SIZE:
        {
            if (wParam == SIZE_MINIMIZED) return 0;
            SetWindowPos(tb.getWindowHandle(), HWND_TOP, 0, 0, LOWORD(lParam), 200, SWP_NOZORDER | SWP_NOMOVE);
            SetWindowPos(edt.getWindowHandle(), HWND_TOP, 0, 200, LOWORD(lParam), HIWORD(lParam) - 200, SWP_NOZORDER | SWP_NOMOVE);
            break;
        }
        case WM_USER+1:
        {
            int i = 0;
            PostMessageW(edt.getWindowHandle(), WM_USER + 1, wParam, lParam);
            return 0;
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

