#include "Editor/editor.hpp"
#include <d3dcompiler.h>

editor::editor(int x, int y, int width, int height) :
	childWindow(x, y, width, height),
	_depthStencilView(nullptr),
	_cam(static_cast<float>(_width) / static_cast<float>(_height)),
	_isDragging(false), _lastPoint({ 0, 0 })
{}

void editor::createChildWindow()
{
	HRESULT hr = S_FALSE;
	D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	ComPtr<ID3D11DepthStencilState> dsState;
	RUN_DX11(_device->CreateDepthStencilState(&dsDesc, &dsState));
	_context->OMSetDepthStencilState(dsState.Get(), 1);

	setDepthStencilView(_width, _height);
	setViewPort(_width, _height);

	_constantBuffer = std::make_unique<constantBuffer<dx::XMMATRIX, ID3D11VertexShader>>
		(
			_device,
			_context,
			_cam.getTransformationMat(),
			1
		);
	_cube = std::make_unique<cube>(_device, _context);
}

LRESULT editor::handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_SIZE:
		{
			WM_SIZEHandler(wParam, lParam);
			setDepthStencilView(_width, _height);
			setViewPort(_width, _height);
			_cam.updateAspectRatio(static_cast<float>(_width) / static_cast<float>(_height));
			return 0;
		}
		case WM_USER + 1:
		{
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			if (_isDragging) {
				POINT currPoint = { LOWORD(lParam), HIWORD(lParam) };
				float dx = 0.004f * (currPoint.x - _lastPoint.x);
				float dy = 0.004f * (currPoint.y - _lastPoint.y);
				_cam.orbit(dy, -dx);
				// _cam.pan(-dx, dy);
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
			this->destroyWindow();
			return 0;
		}
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);	
}

void editor::paint()
{
	_context->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());
	constexpr float color[4] = { 0.1f, 0.1f ,0.1f ,1.0f };
	_context->ClearRenderTargetView(_renderTargetView.Get(), color);
	_context->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	_constantBuffer->update(_cam.getTransformationMat(), 1);
	_constantBuffer->bind();

	_cube->bindAll();
	_cube->draw();
	_swap->Present(1, 0);
}

void editor::setDepthStencilView(int width, int height)
{
	if (_device && _swap) {
		_depthStencilView.Reset();

		ComPtr<ID3D11Texture2D> depthStencil;
		D3D11_TEXTURE2D_DESC dsTexDesc = { 0 };
		dsTexDesc.Width = width;
		dsTexDesc.Height = height;
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
	}
}

void editor::setViewPort(int width, int height)
{
	if (_context) {
		D3D11_VIEWPORT viewPort{
			.TopLeftX = 0,
			.TopLeftY = 0,
			.Width = static_cast<float>(width),
			.Height = static_cast<float>(height),
			.MinDepth = 0,
			.MaxDepth = 1
		};
		_context->RSSetViewports(1, &viewPort);
	}
}

