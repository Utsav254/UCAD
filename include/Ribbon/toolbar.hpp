#pragma once
#include "util/window.hpp"
#include "util/UI.hpp"

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class toolBar : window
{
public:
	static constexpr LPCWSTR className = L"toolBarClass";

	toolBar(HINSTANCE hInst, int x, int y, int width, int height);
	~toolBar() = default;

	void createWindow(const bool showWindow, HWND parent, ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);

	LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	void paintToolbar();

	HWND gethWnd() const { return _hWnd; }

private:
	int _x, _y;
	int _width, _height;

	HWND _hWnd, _hWndParent;

	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _context;
	ComPtr<IDXGISwapChain> _swap;
	ComPtr<ID3D11RenderTargetView>_renderTargetView;

	bool _dirty;

	ui _ui;

private:
	static inline WNDCLASSEX wc =
	{
		.cbSize = sizeof(WNDCLASSEXW),
		.style = CS_CLASSDC,
		.lpfnWndProc = DefWindowProcW,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = nullptr,
		.hIcon = nullptr,
		.hCursor = nullptr,
		.hbrBackground = nullptr,
		.lpszMenuName = nullptr,
		.lpszClassName = className,
		.hIconSm = nullptr,
	};
};

