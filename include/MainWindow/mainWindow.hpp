#pragma once
#include "util/window.hpp"
#include "Ribbon/toolbar.hpp"
#include "Editor/editor.hpp"
#include <d3d11.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class mainWindow : public window {
public:
    static constexpr LPCWSTR className = L"mainWindowClass";

    mainWindow(const int width, const int height, LPCWSTR windowName);

    ~mainWindow() = default;

    void createWindow(const bool showWindow);

    LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

    void renderAll() {
        tb.paintToolbar();
        edt.paintEditor();
    }

public:
    inline const HWND getWindowHandle() const { return _hWnd; }

private:
    toolBar tb;
    editor edt;

    int _height, _width;
    LPCWSTR _windowName;
    HWND _hWnd;

    ComPtr<ID3D11Device> _device;
    ComPtr<ID3D11DeviceContext> _context;

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
