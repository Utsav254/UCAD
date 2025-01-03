#pragma once
#include "util/window.hpp"
#include "util/UI.hpp"

class toolBar : window
{
public:
	static constexpr LPCWSTR className = L"toolBarClass";

	toolBar(HINSTANCE hInst, int x, int y, int width, int height);
	~toolBar() {
		if (_renderTargetView != nullptr) _renderTargetView->Release();
		if (_swap != nullptr) _swap->Release();
	}

	void createWindow(const bool showWindow, HWND parent, ID3D11Device* device, ID3D11DeviceContext* context);

	LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	void paintToolbar();

	HWND gethWnd() const { return _hWnd; }

private:
	int _x, _y;
	int _width, _height;

	HWND _hWnd, _hWndParent;

	ID3D11Device* _device;
	ID3D11DeviceContext* _context;
	IDXGISwapChain* _swap;
	ID3D11RenderTargetView*_renderTargetView;

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

