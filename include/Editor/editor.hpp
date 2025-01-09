#pragma once
#include "util/childWindow.hpp"
#include "Editor/BindableDrawable/cube.hpp"
#include "Editor/camera.hpp"
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
using Microsoft::WRL::ComPtr;
namespace dx = DirectX;

class editor : public childWindow
{
public:

	editor(int x, int y, int width, int height);
	~editor() = default;

	void createChildWindow() override;

	LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	void paint() override;

	inline const HWND getWindowHandle() const { return _hWnd; }

private:
	void setDepthStencilView(int width, int height);
	void setViewPort(int widht, int height);

private:
	// inherits device context swap-chain render-target-view
	ComPtr<ID3D11DepthStencilView> _depthStencilView;
	std::unique_ptr<constantBuffer<dx::XMMATRIX, ID3D11VertexShader>> _constantBuffer;

	camera _cam;
	std::unique_ptr<cube> _cube;

	bool _isDragging;
	POINT _lastPoint;
};