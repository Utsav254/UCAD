#pragma once
#include <d3d11.h>
#include "util/win.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class ui
{
private:
	ImGuiContext* _uiContext;

public:
	ImGuiIO& io;

public:
	ui();
	~ui();

	void initialise(HWND hWnd, ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);

	void newFrame();

	void endFrame(ComPtr<ID3D11DeviceContext> context, ComPtr<ID3D11RenderTargetView> renderTargetView, const float color[4]);
};