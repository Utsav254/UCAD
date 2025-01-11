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
		case WM_NCHITTEST:
		{
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			ScreenToClient(hWnd, &pt);
			if (pt.y < 50) return HTTRANSPARENT;
			break;
		}
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

	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(static_cast<float>(_width - 10), static_cast<float>(_height - 10)), ImGuiCond_Always);
	ImGui::Begin("Ribbon", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);

	ImGui::Text("This is inside a transparent window!");
	ImGui::Text("Place Holder for UI");
	if (ImGui::Button("X", {50.0f ,50.0f})) PostMessageW(_hWndParent, WM_CLOSE, 0, 0);

	ImGui::End();

	const float color[4] = { 0.0f , 0.0f, 0.0f, 1.0f };
	_ui.endFrame(_context, _renderTargetView, color);
	_swap->Present(1, 0);
}
