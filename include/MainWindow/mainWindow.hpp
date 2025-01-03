#pragma once
#include "util/window.hpp"
#include "Ribbon/toolbar.hpp"
#include <wrl/client.h>
#include <d3d11.h>

using Microsoft::WRL::ComPtr;

class mainWindow : public window {
public:
    static constexpr LPCWSTR className = L"mainWindowClass";

    mainWindow(const int width, const int height, LPCWSTR windowName, HINSTANCE hInst);

    ~mainWindow() {
        if (_context != nullptr) _context->Release();
        if (_device != nullptr) _device->Release();
    }

    void createWindow(const bool showWindow);

    LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

    void renderAll() {
        tb.paintToolbar();
    }

public:
    inline const HWND getWindowHandle() const { return _hWnd; }

private:
    toolBar tb;

    int _height, _width;
    LPCWSTR _windowName;
    HWND _hWnd;

    ID3D11Device* _device;
    ID3D11DeviceContext* _context;

private:
    static inline WNDCLASSEX wc =
    {
        .cbSize = sizeof(WNDCLASSEXW),
        .style = CS_OWNDC,
        .lpfnWndProc = DefWindowProcW,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = nullptr,
        .hIcon = nullptr,
        .hCursor = LoadCursorW(nullptr, IDC_ARROW),
        .hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH),
        .lpszMenuName = nullptr,
        .lpszClassName = className,
        .hIconSm = nullptr,
    };
};
