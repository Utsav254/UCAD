#include "Editor/editor.hpp"
#include <d3dcompiler.h>

editor::editor(int x, int y, int width, int height) :
	window(&wc),
	_x(x), _y(y),
	_width(width), _height(height),
	_hWnd(nullptr), _hWndParent(nullptr),
	_device(nullptr), _context(nullptr), _swap(nullptr), _renderTargetView(nullptr),
	_model(dx::XMMatrixIdentity()), 
	_view(dx::XMMatrixLookAtLH(
		dx::XMVectorSet(2.0f, 2.0f, 2.0f, 1.0f),
		dx::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
		dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	)),
	_projection(dx::XMMatrixPerspectiveFovLH(
		dx::XM_PIDIV4, 
		static_cast<float>(_width / _height),
		0.1f,
		100.0f
	))
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

			if (_device && _swap && _renderTargetView) {
				_renderTargetView->Release();

				RUN_DX11_NOTHROW(_swap->ResizeBuffers(0, _width, _height, DXGI_FORMAT_UNKNOWN, 0));

				ID3D11Texture2D* pBackBuffer;
				RUN_DX11_NOTHROW(_swap->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)));
				if (pBackBuffer != nullptr) {
					RUN_DX11_NOTHROW(_device->CreateRenderTargetView(pBackBuffer, nullptr, _renderTargetView.GetAddressOf()));
				}
				pBackBuffer->Release();
			}

			_projection = dx::XMMatrixPerspectiveFovLH(
				dx::XM_PIDIV4,
				static_cast<float>(_width / _height),
				0.1f,
				100.0f
			);
			return 0;
		}
		case WM_USER + 1:
		{
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
	HRESULT hr = S_FALSE;
	const float color[4] = { 0.1f, 0.1f ,0.1f ,1.0f };
	_context->ClearRenderTargetView(_renderTargetView.Get(), color);

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

	const vertex vertices[] =
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

	const unsigned short indices[] =
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

	// bind render target
	_context->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);

	// set primitive topology
	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// bind view port
	D3D11_VIEWPORT viewPort
	{
		.TopLeftX = 0,
		.TopLeftY = 0,
		.Width = static_cast<float>(_width),
		.Height = static_cast<float>(_height),
		.MinDepth = 0,
		.MaxDepth = 1
	};
	_context->RSSetViewports(1, &viewPort);

	// draw call
	_context->DrawIndexed(std::size(indices), 0, 0);

	// flip front back buffer
	_swap->Present(1, 0);
}

