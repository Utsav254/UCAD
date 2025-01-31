#include "pch.hpp"
#include "window.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

window::window() :
	_height(0), _width(0),
	_hWnd(nullptr),
	_device(nullptr), _context(nullptr), _swap(nullptr),
	_renderTargetView(nullptr), _depthStencilView(nullptr),
	_ImguiContext(ImGui::CreateContext()), _io(ImGui::GetIO()),
	_cam(1.0f)
{
#ifdef _DEBUG
	static unsigned int numCalls = 0;
	numCalls++;
	if (numCalls > 1) throw ERROR_FMT_M("Main Window error ... attempting to call createWindow twice on same instance");
#endif // _DEBUG

	if (RegisterClassExW(&wc) == 0) throw WINDOW_ERROR;

	_hWnd = CreateWindowExW
	(
		WS_EX_APPWINDOW,
		className, windowName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr, nullptr,
		_hInst, this
	);
	if (!_hWnd) throw WINDOW_ERROR;
	RECT clientRect;
	if (!GetClientRect(_hWnd, &clientRect)) throw WINDOW_ERROR;
	_width = clientRect.right - clientRect.left;
	_height = clientRect.bottom - clientRect.top;

	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
#ifdef _DEBUG
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT createDeviceFlags = 0;
#endif //_DEBUG

	static const DXGI_SWAP_CHAIN_DESC sd =
	{
		.BufferDesc =
		{
			.Width = 0,
			.Height = 0,
			.RefreshRate = {.Numerator = 0, .Denominator = 0},
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
			.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
			.Scaling = DXGI_MODE_SCALING_UNSPECIFIED
		},
		.SampleDesc = {.Count = 1, .Quality = 0},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 2,
		.OutputWindow = _hWnd,
		.Windowed = TRUE,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	};

	HRESULT hr = S_FALSE;
	RUN_DX11(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
		featureLevelArray,
		_countof(featureLevelArray),
		D3D11_SDK_VERSION,
		&sd,
		&_swap,
		&_device,
		&featureLevel,
		&_context
	));

	ID3D11Texture2D* pBackBuffer;
	RUN_DX11(_swap->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)));
	if (pBackBuffer != nullptr) RUN_DX11(_device->CreateRenderTargetView(pBackBuffer, nullptr, &_renderTargetView));
	pBackBuffer->Release();
	setDepthStencilView();
	setViewPort();

	_cam.updateAspectRatio(static_cast<float>(_width) / static_cast<float>(_height));
	_constantBuffer = std::make_unique<constantBuffer<dx::XMMATRIX, ID3D11VertexShader>>
		(
			_device,
			_context,
			_cam.getTransformationMat(),
			1
		);
	_cb = std::make_unique<CubeDemo>(_device, _context);

	IMGUI_CHECKVERSION();
	_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(_hWnd);
	ImGui_ImplDX11_Init(_device.Get(), _context.Get());

	ShowWindow(_hWnd, SW_SHOWNORMAL);
	UpdateWindow(_hWnd);
}

window::~window()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	UnregisterClassW(className, _hInst);
}

LRESULT window::handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	if (_io.WantCaptureMouse && (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MOUSEMOVE)) {
		return 0;
	}

	switch (msg)
	{
		case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED) return 0;
			_width = (UINT)LOWORD(lParam);
			_height = (UINT)HIWORD(lParam);
			setRenderTargetView();
			setDepthStencilView();
			setViewPort();
			_cam.updateAspectRatio(static_cast<float>(_width) / static_cast<float>(_height));
			break;
		}
		case WM_MOUSEMOVE:
		{
			if (_isDragging) {
				POINT currPoint = { LOWORD(lParam), HIWORD(lParam) };
				_cam.mouseClickDrag(static_cast<float>(currPoint.x - _lastPoint.x), static_cast<float>(currPoint.y - _lastPoint.y));
				_lastPoint = currPoint;
			}
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			_isDragging = true;
			_lastPoint = { LOWORD(lParam), HIWORD(lParam) };
			SetCapture(hWnd);

			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = hWnd;
			tme.dwHoverTime = HOVER_DEFAULT;
			TrackMouseEvent(&tme);

			return 0;
		}
		case WM_LBUTTONUP:
		{
			if (_isDragging) {
				_isDragging = false;
				ReleaseCapture();
			}
			return 0;
		}
		case WM_MOUSELEAVE:
		{
			if (_isDragging) {
				_isDragging = false;
				ReleaseCapture();
			}
			return 0;
		}
		case WM_MOUSEWHEEL:
		{
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			_cam.forwardBackward(0.004f * static_cast<float>(zDelta));
			return 0;
		}
		case WM_CLOSE:
		{
			DestroyWindow(hWnd);
			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

void window::paint()
{
	constexpr float color[4] = { 0.7f, 0.7f ,0.7f ,1.0f };
	_context->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());
	_context->ClearRenderTargetView(_renderTargetView.Get(), color);
	_context->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	_constantBuffer->update(_cam.getTransformationMat(), 1);
	_constantBuffer->bind();

	//_cb->bindAll();
	_cb->render();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (ImGui::Begin("Tools")){
		_cam.drawUI();
		ImGui::Text("%.1f", _io.Framerate);
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	_swap->Present(1, 0);
}

void window::setDepthStencilView()
{
	HRESULT hr = S_FALSE;
	if (_device && _swap) {
		_depthStencilView.Reset();
		ID3D11Texture2D *depthStencil;
		const D3D11_TEXTURE2D_DESC dsTexDesc =
		{
			.Width = _width,
			.Height = _height,
			.MipLevels = 1,
			.ArraySize = 1,
			.Format = DXGI_FORMAT_D32_FLOAT,
			.SampleDesc = {.Count = 1, .Quality = 0},
			.Usage = D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_DEPTH_STENCIL,
			.CPUAccessFlags = 0,
			.MiscFlags = 0
		};
		RUN_DX11(_device->CreateTexture2D(&dsTexDesc, nullptr, &depthStencil));

		D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc = {};
		dsViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsViewDesc.Texture2D.MipSlice = 0;
		RUN_DX11(_device->CreateDepthStencilView(depthStencil, &dsViewDesc, _depthStencilView.GetAddressOf()));
		depthStencil->Release();
	}
}

void window::setViewPort()
{
	if (_context) {
		const D3D11_VIEWPORT viewPort{
			.TopLeftX = 0,
			.TopLeftY = 0,
			.Width = static_cast<float>(_width),
			.Height = static_cast<float>(_height),
			.MinDepth = 0,
			.MaxDepth = 1
		};
		_context->RSSetViewports(1, &viewPort);
	}
}

void window::setRenderTargetView()
{
	HRESULT hr = S_FALSE;
	if (_device && _swap && _renderTargetView) {
		_renderTargetView.Reset();
		RUN_DX11(_swap->ResizeBuffers(0, _width, _height, DXGI_FORMAT_UNKNOWN, 0));
		ID3D11Texture2D* pBackBuffer;
		RUN_DX11(_swap->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)));
		if (pBackBuffer != nullptr) {
			RUN_DX11(_device->CreateRenderTargetView(pBackBuffer, nullptr, &_renderTargetView));
		}
		pBackBuffer->Release();
	}
}

LRESULT CALLBACK window::handleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (msg == WM_NCCREATE) {
		const CREATESTRUCTW* const tempCreateStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
		window* const tempWindow = static_cast<window*>(tempCreateStruct->lpCreateParams);

		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(tempWindow));
		SetWindowLongPtrW(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&window::handleMessageRedirect));
		return tempWindow->handleMessage(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

inline LRESULT CALLBACK window::handleMessageRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	window* const tempWindow = reinterpret_cast<window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	try {
		return tempWindow->handleMessage(hWnd, msg, wParam, lParam);
	}
	catch (const error& e) {
		e.display();
		return false;
	}
}