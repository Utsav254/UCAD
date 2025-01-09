#pragma once
#include "util/window.hpp"
#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class childWindow : public window
{
public:
	static constexpr LPCWSTR className = L"childWindow";

	childWindow(int x, int y, int width, int height);

	~childWindow() = default;

	void createWindow(const bool showWindow, HWND parent,
		ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context) override final;

protected:
	virtual void createChildWindow() = 0;

	LRESULT WM_SIZEHandler(WPARAM wParam, LPARAM lParam);

protected:
	int _x, _y;
	int _width, _height;

	HWND _hWnd, _hWndParent;

	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _context;
	ComPtr<IDXGISwapChain> _swap;
	ComPtr<ID3D11RenderTargetView>_renderTargetView;

private:
	static inline WNDCLASSEX wc =
	{
		.cbSize = sizeof(WNDCLASSEXW),
		.style = CS_HREDRAW | CS_VREDRAW,
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