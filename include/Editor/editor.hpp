#pragma once
#include "util/window.hpp"
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
using Microsoft::WRL::ComPtr;
namespace dx = DirectX;

class editor : public window
{
public:
	static constexpr LPCWSTR className = L"editorClass";

	editor(int x, int y, int width, int height);
	~editor() = default;

	void createWindow(const bool showWindow, HWND parent, ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);

	LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	void paintEditor();

	inline const HWND getWindowHandle() const { return _hWnd; }

private:
	int _x, _y;
	int _width, _height;

	HWND _hWnd, _hWndParent;

	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _context;
	ComPtr<IDXGISwapChain> _swap;
	ComPtr<ID3D11RenderTargetView>_renderTargetView;

	dx::XMVECTOR _cameraPos;

	dx::XMMATRIX _model;
	dx::XMMATRIX _view;
	dx::XMMATRIX _projection;

	bool _isDragging;
	POINT _lastPoint;

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