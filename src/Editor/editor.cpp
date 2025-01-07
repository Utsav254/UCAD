#include "Editor/editor.hpp"
#include <d3dcompiler.h>

editor::editor(int x, int y, int width, int height) :
	window(&wc),
	_x(x), _y(y),
	_width(width), _height(height),
	_hWnd(nullptr), _hWndParent(nullptr),
	_device(nullptr), _context(nullptr), _swap(nullptr), _renderTargetView(nullptr),
	_cameraPos(dx::XMVectorSet(2.0f, 2.0f, 2.0f, 1.0f)),
	_model(dx::XMMatrixIdentity()),
	_view(dx::XMMatrixLookAtLH(
		_cameraPos,
		dx::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
		dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	)),
	_projection(dx::XMMatrixPerspectiveFovLH(
		dx::XM_PIDIV4,
		static_cast<float>(_width / _height),
		0.1f,
		100.0f
	)),
	_isDragging(false), _lastPoint({ 0, 0 })
{}

void editor::createWindow(const bool showWindow, HWND parent, ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	if (parent == nullptr) throw ERROR_FMT_M("editor was given nullptr parent handle");
	_hWndParent = parent;

	if (device == nullptr) throw ERROR_FMT_M("editor was given nullptr d3d11 device");
	_device = device;

	if (context == nullptr) throw ERROR_FMT_M("editor was give nullptr d3d11 context");
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

	D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	ComPtr<ID3D11DepthStencilState> dsState;
	RUN_DX11(_device->CreateDepthStencilState(&dsDesc, &dsState));
	_context->OMSetDepthStencilState(dsState.Get(), 1);

	ComPtr<ID3D11Texture2D> depthStencil;
	D3D11_TEXTURE2D_DESC dsTexDesc = { 0 };
	dsTexDesc.Width = _width;
	dsTexDesc.Height = _height;
	dsTexDesc.MipLevels = 1;
	dsTexDesc.ArraySize = 1;
	dsTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsTexDesc.SampleDesc.Count = 1;
	dsTexDesc.SampleDesc.Quality = 0;
	dsTexDesc.Usage = D3D11_USAGE_DEFAULT;
	dsTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	RUN_DX11(_device->CreateTexture2D(&dsTexDesc, nullptr, &depthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc = {};
	dsViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsViewDesc.Texture2D.MipSlice = 0;
	RUN_DX11(_device->CreateDepthStencilView(depthStencil.Get(), &dsViewDesc, &_depthStencilView));

	if (showWindow) ShowWindow(_hWnd, SW_SHOWDEFAULT);
	UpdateWindow(_hWnd);
}

LRESULT editor::handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED) return 0;
			_width = (UINT)LOWORD(lParam);
			_height = (UINT)HIWORD(lParam);

			if (_width == 0 || _height == 0) return 0;

			if (_device && _swap && _renderTargetView) {
				// Release both render target and depth stencil views
				_renderTargetView.Reset();
				_depthStencilView.Reset();

				RUN_DX11_NOTHROW(_swap->ResizeBuffers(0, _width, _height, DXGI_FORMAT_UNKNOWN, 0));

				// Recreate render target view
				ID3D11Texture2D* pBackBuffer;
				RUN_DX11_NOTHROW(_swap->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)));
				if (pBackBuffer != nullptr) {
					RUN_DX11_NOTHROW(_device->CreateRenderTargetView(pBackBuffer, nullptr, _renderTargetView.GetAddressOf()));
					pBackBuffer->Release();
				}

				// Recreate depth stencil texture and view
				ComPtr<ID3D11Texture2D> depthStencil;
				D3D11_TEXTURE2D_DESC dsTexDesc = { 0 };
				dsTexDesc.Width = _width;
				dsTexDesc.Height = _height;
				dsTexDesc.MipLevels = 1;
				dsTexDesc.ArraySize = 1;
				dsTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
				dsTexDesc.SampleDesc.Count = 1;
				dsTexDesc.SampleDesc.Quality = 0;
				dsTexDesc.Usage = D3D11_USAGE_DEFAULT;
				dsTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
				RUN_DX11_NOTHROW(_device->CreateTexture2D(&dsTexDesc, nullptr, &depthStencil));

				D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc = {};
				dsViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
				dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				dsViewDesc.Texture2D.MipSlice = 0;
				RUN_DX11_NOTHROW(_device->CreateDepthStencilView(depthStencil.Get(), &dsViewDesc, _depthStencilView.GetAddressOf()));

				// Reset viewport with new dimensions
				D3D11_VIEWPORT viewPort{
					.TopLeftX = 0,
					.TopLeftY = 0,
					.Width = static_cast<float>(_width),
					.Height = static_cast<float>(_height),
					.MinDepth = 0,
					.MaxDepth = 1
				};
				_context->RSSetViewports(1, &viewPort);
			}

			_projection = dx::XMMatrixPerspectiveFovLH(
				dx::XM_PIDIV4,
				static_cast<float>(_width) / static_cast<float>(_height),
				0.1f,
				100.0f
			);

			return 0;
		}
		case WM_USER + 1:
		{
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			SetCapture(hWnd);
			_isDragging = true;
			_lastPoint.x = LOWORD(lParam);
			_lastPoint.y = HIWORD(lParam);
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			if (_isDragging) {
				POINT currPoint = { LOWORD(lParam), HIWORD(lParam) };

				float dx = DirectX::XMConvertToRadians(0.25f * (currPoint.x - _lastPoint.x));
				float dy = DirectX::XMConvertToRadians(0.25f * (currPoint.y - _lastPoint.y));

				// Assuming _cameraPos.x = azimuth, _cameraPos.y = pitch, _cameraPos.z = radius
				DirectX::XMFLOAT3 spherical;
				DirectX::XMStoreFloat3(&spherical, _cameraPos);

				spherical.x -= dx;  // Adjust azimuth (horizontal rotation)
				spherical.y = std::max(-DirectX::XM_PIDIV2 + 0.01f,
					std::min(spherical.y + dy, DirectX::XM_PIDIV2 - 0.01f));

				_cameraPos = DirectX::XMLoadFloat3(&spherical);

				// Calculate camera position in world space
				DirectX::XMVECTOR pos = DirectX::XMVectorSet(
					spherical.z * std::cos(spherical.y) * std::cos(spherical.x),
					spherical.z * std::sin(spherical.y),
					spherical.z * std::cos(spherical.y) * std::sin(spherical.x),
					1.0f
				);

				// Update view matrix
				_view = DirectX::XMMatrixLookAtLH(
					pos,  // Camera position in world space
					DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),  // Looking at origin
					DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)   // Up vector
				);

				_lastPoint = currPoint;
			}
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
		case WM_MOUSEWHEEL:
		{
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			DirectX::XMFLOAT3 spherical;
			DirectX::XMStoreFloat3(&spherical, _cameraPos);
			float zoomSpeed = 0.01f;
			spherical.z -= (zDelta * zoomSpeed);
			spherical.z = std::max(1.0f, std::min(spherical.z, 100.0f));

			_cameraPos = DirectX::XMLoadFloat3(&spherical);

			DirectX::XMVECTOR pos = DirectX::XMVectorSet(
				spherical.z * std::cos(spherical.y) * std::cos(spherical.x),
				spherical.z * std::sin(spherical.y),
				spherical.z * std::cos(spherical.y) * std::sin(spherical.x),
				1.0f
			);

			_view = DirectX::XMMatrixLookAtLH(
				pos,  // Camera position in world space
				DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),  // Looking at origin
				DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)   // Up vector
			);
			return 0;
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

void editor::paintEditor()
{
	_context->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());

	HRESULT hr = S_FALSE;
	const float color[4] = { 0.1f, 0.1f ,0.1f ,1.0f };
	_context->ClearRenderTargetView(_renderTargetView.Get(), color);
	_context->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	// verticies definitions
	struct vertex
	{
		struct {
			float x;
			float y;
			float z;
		} pos;
		
		struct colour {
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		} col;
	};

	constexpr vertex vertices[] =
	{
		{{-0.5f, -0.5f, -0.5f}, {255,   0,   0, 255}},
		{{ 0.5f, -0.5f, -0.5f}, {  0, 255,   0, 255}},
		{{-0.5f,  0.5f, -0.5f}, {  0,   0, 255, 255}},
		{{ 0.5f,  0.5f, -0.5f}, {0  , 255, 255, 255}},

		{{-0.5f, -0.5f,  0.5f}, {255,   0, 255, 255}},
		{{ 0.5f, -0.5f,  0.5f}, {255, 255,   0, 255}},
		{{-0.5f,  0.5f,  0.5f}, {127, 127, 127, 255}},
		{{ 0.5f,  0.5f,  0.5f}, {0  , 127, 127, 255}},
	};

	constexpr unsigned short indices[] =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4,
	};

	dx::XMMATRIX mvpMatrix = dx::XMMatrixTranspose(_model * _view * _projection);

	//create and attach vertex buffer
	ComPtr<ID3D11Buffer> vertexBuffer;
	constexpr D3D11_BUFFER_DESC bufferDesc = 
	{
		.ByteWidth = sizeof(vertices),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = sizeof(vertex)
	};
	const D3D11_SUBRESOURCE_DATA subResData = {.pSysMem = vertices};
	RUN_DX11(_device->CreateBuffer(&bufferDesc, &subResData, &vertexBuffer));
	const UINT stride = sizeof(vertex);
	const UINT offset = 0;
	_context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// create and attach index buffer
	ComPtr<ID3D11Buffer> indexBuffer;
	constexpr D3D11_BUFFER_DESC iBufferDesc =
	{
		.ByteWidth = sizeof(indices),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = sizeof(unsigned short)
	};
	const D3D11_SUBRESOURCE_DATA indexsubResData = { .pSysMem = indices };
	RUN_DX11(_device->CreateBuffer(&iBufferDesc, &indexsubResData, &indexBuffer));
	_context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// load and create vertex shader object
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3DBlob> Blob;
	RUN_DX11(D3DReadFileToBlob(L"exe/x64/Debug/VertexShader.cso", &Blob));
	RUN_DX11(_device->CreateVertexShader(
		Blob->GetBufferPointer(), Blob->GetBufferSize(),
		nullptr, &vertexShader
	));
	_context->VSSetShader(vertexShader.Get(), nullptr, 0);


	// set vertex shader constant buffer with mvp matrices
	ComPtr<ID3D11Buffer> vsConstantBuffer;
	constexpr D3D11_BUFFER_DESC vsConstBufferDesc =
	{
		.ByteWidth = sizeof(dx::XMMATRIX),
		.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags = 0,
		.StructureByteStride = sizeof(unsigned short)
	};
	const D3D11_SUBRESOURCE_DATA constBufSubResData = { .pSysMem = &mvpMatrix };
	RUN_DX11(_device->CreateBuffer(&vsConstBufferDesc, &constBufSubResData, &vsConstantBuffer));
	_context->VSSetConstantBuffers(0, 1, vsConstantBuffer.GetAddressOf());

	// input layout object
	ComPtr<ID3D11InputLayout> inputLayout;
	const D3D11_INPUT_ELEMENT_DESC inElDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	RUN_DX11(_device->CreateInputLayout(inElDesc, std::size(inElDesc),
		Blob->GetBufferPointer(), Blob->GetBufferSize(),
		&inputLayout));
	_context->IASetInputLayout(inputLayout.Get());

	// load and create pixel shader object
	ComPtr<ID3D11PixelShader> pixelShader;
	RUN_DX11(D3DReadFileToBlob(L"exe/x64/Debug/PixelShader.cso", &Blob));
	RUN_DX11(_device->CreatePixelShader(
		Blob->GetBufferPointer(), Blob->GetBufferSize(),
		nullptr, &pixelShader
	));
	_context->PSSetShader(pixelShader.Get(), nullptr, 0); 

	// set primitive topology
	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw call
	_context->DrawIndexed(std::size(indices), 0, 0);

	// flip front back buffer
	_swap->Present(1, 0);
}

