#pragma once
#include "util/childWindow.hpp"
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
	ComPtr<ID3D11DepthStencilView> _depthStencilView;

	dx::XMVECTOR _cameraPos;
	dx::XMMATRIX _model;
	dx::XMMATRIX _view;
	dx::XMMATRIX _projection;

	bool _isDragging;
	POINT _lastPoint;
};