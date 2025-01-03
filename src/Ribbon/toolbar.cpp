#include "Ribbon/toolbar.hpp"

toolBar::toolBar(HINSTANCE hInst, int x, int y, int width, int height) :
	window(&wc),
	_x(x), _y(y),
	_width(width), _height(height),
	_hWnd(nullptr), _hWndParent(nullptr),
	_device(nullptr), _context(nullptr), _swap(nullptr),  _renderTargetView(nullptr),
	_dirty(true)
{}

void toolBar::createWindow(const bool showWindow, HWND parent, ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
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
	sd.BufferCount = 1;
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
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	IDXGIFactory* factory = nullptr;
	HRESULT res = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(res)) {
		throw WINDOW_ERROR_HR(res);
	}

	res = factory->CreateSwapChain(_device.Get(), &sd, &_swap);
	if (FAILED(res)) throw WINDOW_ERROR_HR(res);

	ID3D11Texture2D* pBackBuffer;
	res = _swap->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (FAILED(res)) throw WINDOW_ERROR_HR(res);

	res = _device->CreateRenderTargetView(pBackBuffer, nullptr, &_renderTargetView);
	if (FAILED(res)) throw WINDOW_ERROR_HR(res);

	pBackBuffer->Release();
	factory->Release();

	_ui.initialise(_hWnd, _device, _context);

	if (showWindow) ShowWindow(_hWnd, SW_SHOWDEFAULT);
	UpdateWindow(_hWnd);
}

LRESULT toolBar::handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
		case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED) return 0;
			_width = (UINT)LOWORD(lParam);
			_height = (UINT)HIWORD(lParam);

			if (_device && _swap && _renderTargetView) {
				_renderTargetView->Release();

				_swap->ResizeBuffers(0, _width, _height, DXGI_FORMAT_UNKNOWN, 0);

				ID3D11Texture2D* pBackBuffer;
				_swap->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
				if(pBackBuffer != nullptr) _device->CreateRenderTargetView(pBackBuffer, nullptr, _renderTargetView.GetAddressOf());
				pBackBuffer->Release();
			}

			_dirty = true;
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			if (!_dirty) {
				_dirty = true;
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hWnd;
				TrackMouseEvent(&tme);
			}
			break;
		}
		case WM_MOUSELEAVE:
		{
			_dirty = false;
			break;
		}
		case WM_CLOSE:
		{
			DestroyWindow(hWnd);
			return 0;
		}
		case WM_DESTROY:
		{
			this->destroyWindow();
			return 0;
		}
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

void toolBar::paintToolbar() {
	if (_dirty == false) return;

	_ui.newFrame();

	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::SetNextWindowSize(ImVec2((float)_width, (float)_height), ImGuiCond_Always);
	ImGui::Begin("ToolBar", nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("what a nice ribbon innit?");

	float fps = ImGui::GetIO().Framerate;

	ImGui::Text("FPS: %.1f", fps);

	ImGui::Button("Button tester1", { 100,100 });

	ImGui::End();

	const float color[4] = { 0.0f , 0.0f, 0.0f, 1.0f };
	_ui.endFrame(_context, _renderTargetView, color);

	_swap->Present(1, 0);
}
