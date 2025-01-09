#include "Ribbon/toolbar.hpp"

toolBar::toolBar(int x, int y, int width, int height) :
	childWindow(x, y, width, height),
	_dirty(true),
	_color(0.0f, 0.0f, 0.0f, 0.0f)
{}

void toolBar::createChildWindow()
{
	_ui.initialise(_hWnd, _device, _context);
}

LRESULT toolBar::handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
		case WM_SIZE:
		{
			_dirty = true;
			return WM_SIZEHandler(wParam, lParam);
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

void toolBar::paint() {
#ifdef _NDEBUG
	if (_dirty == false) return;
#endif

	_ui.newFrame();

	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::SetNextWindowSize(ImVec2((float)_width, (float)_height), ImGuiCond_Always);
	ImGui::Begin("ToolBar", nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_AlwaysAutoResize);

	float fps = ImGui::GetIO().Framerate;

	ImGui::Text("Select Your Colour, FPS %f:", fps);
	if (ImGui::ColorEdit3("Colour Picker##1", (float*)&_color, 0)) {
		PostMessageW(_hWndParent, WM_USER + 1, (WPARAM)(&_color), 0);
	}
	ImGui::Text("theta %f:", theta);

	ImGui::End();
	
	const float color[4] = { 0.0f , 0.0f, 0.0f, 1.0f };
	_ui.endFrame(_context, _renderTargetView, color);

	_swap->Present(1, 0);
}
