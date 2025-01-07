#include "util/childWindow.hpp"

childWindow::childWindow(int x, int y, int width, int height):
	window(&wc), 
	_x(x), _y(y),
	_width(width), _height(height),
	_hWnd(nullptr), _hWndParent(nullptr),
	_device(nullptr), _context(nullptr), _swap(nullptr), _renderTargetView(nullptr)
{}

void childWindow::createWindow(const bool showWindow, HWND parent,
	ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	if (parent == nullptr) throw ERROR_FMT_M("toolbar was given nullptr parent handle");
	_hWndParent = parent;

	if (device == nullptr) throw ERROR_FMT_M("toolbar was given nullptr d3d11 device");
	_device = device;

	if (context == nullptr) throw ERROR_FMT_M("toolbar was give nullptr d3d11 context");
	_context = context;

	_hWnd = CreateWindowExW
	(
		0,
		className,
		L"ToolBar",
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
		_x, _y, _width, _height,
		parent, nullptr,
		_hInst, this
	);

	if (!_hWnd) throw WINDOW_ERROR;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = _hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	HRESULT hr = S_FALSE;

	IDXGIFactory* factory = nullptr;
	RUN_DX11(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory));

	RUN_DX11(factory->CreateSwapChain(_device.Get(), &sd, &_swap));

	ID3D11Texture2D* pBackBuffer;
	RUN_DX11(_swap->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)));

	RUN_DX11(_device->CreateRenderTargetView(pBackBuffer, nullptr, &_renderTargetView));

	pBackBuffer->Release();
	factory->Release();

	// call child window override creator function
	createChildWindow();

	if (showWindow) ShowWindow(_hWnd, SW_SHOWDEFAULT);
	UpdateWindow(_hWnd);
}

LRESULT childWindow::WM_SIZEHandler(WPARAM wParam, LPARAM lParam)
{
	if (wParam == SIZE_MINIMIZED) return 0;
	_width = (UINT)LOWORD(lParam);
	_height = (UINT)HIWORD(lParam);

	if (_device && _swap && _renderTargetView) {
		_renderTargetView.Reset();

		RUN_DX11_NOTHROW(_swap->ResizeBuffers(0, _width, _height, DXGI_FORMAT_UNKNOWN, 0));

		ID3D11Texture2D* pBackBuffer;
		RUN_DX11_NOTHROW(_swap->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)));
		if (pBackBuffer != nullptr) {
			RUN_DX11_NOTHROW(_device->CreateRenderTargetView(pBackBuffer, nullptr, _renderTargetView.GetAddressOf()));
		}
		pBackBuffer->Release();
	}
	return 0;
}

