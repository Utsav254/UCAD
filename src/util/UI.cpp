#include "util/UI.hpp"

ui::ui():
	_uiContext(ImGui::CreateContext()),
	io(ImGui::GetIO())
{
	IMGUI_CHECKVERSION();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
}

ui::~ui()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext(_uiContext);
}

void ui::initialise(HWND hWnd, ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(device.Get(), context.Get());
}

void ui::newFrame()
{
	ImGui::SetCurrentContext(_uiContext);
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ui::endFrame(ComPtr<ID3D11DeviceContext> context, ComPtr<ID3D11RenderTargetView> renderTargetView, const float color[4])
{
	ImGui::Render();
	context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);
	context->ClearRenderTargetView(renderTargetView.Get(), color);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
