#pragma once
#include <d3d11.h>
#include "util/win.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

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

	void initialise(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context);

	void newFrame();

	void endFrame(ID3D11DeviceContext* context, ID3D11RenderTargetView* renderTargetView, const float color[4]);
};