#pragma once
#include "error.hpp"
#include "cube.hpp"
#include "dxAbstraction/buffers.hpp"
#include "camera.hpp"
#include "cubeEntity.hpp"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <memory>
#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class window {
public:
	static constexpr LPCWSTR className = L"UpsilonCadWindowClass";
	static constexpr LPCWSTR windowName = L"Upsilon";

	window();
	~window();
	LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void paint();

private:
	static LRESULT CALLBACK handleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static inline LRESULT CALLBACK handleMessageRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	void setRenderTargetView();
	void setDepthStencilView();
	void setViewPort();

private:
	UINT _height, _width;
	HWND _hWnd;

	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _context;
	ComPtr<IDXGISwapChain> _swap;
	ComPtr<ID3D11RenderTargetView>_renderTargetView;
	ComPtr<ID3D11DepthStencilView> _depthStencilView;

	ImGuiContext* _ImguiContext;
	ImGuiIO& _io;

	camera _cam;
	std::unique_ptr<constantBuffer<dx::XMMATRIX, ID3D11VertexShader>> _constantBuffer;
	//std::unique_ptr<cube> _cb;

	std::unique_ptr<CubeDemo> _cb;

	bool _isDragging;
	POINT _lastPoint;

private:
	static inline HINSTANCE _hInst = GetModuleHandleW(nullptr);
	static inline WNDCLASSEXW wc =
	{
		.cbSize = sizeof(WNDCLASSEXW),
		.style = CS_OWNDC,
		.lpfnWndProc = handleMessageSetup,
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
