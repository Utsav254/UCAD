#pragma once
#include "win32/bases/button.hpp"

class buttonSimple : public button {
public:
	buttonSimple(
		HINSTANCE hInst,
		const int width, const int height,
		const int x, const int y,
		const int buttonId):
		
		button(hInst, width, height, x, y, buttonId)
	{}

	inline void onPaint(HDC hdc, const RECT* const rect, const uint32_t s) const override
	{
		COLORREF cl = 0;

		if (s == button::NOT_ON) cl = RGB(31, 31, 31);
		else if (s == button::HOVER) cl = RGB(255, 0, 0);
		else if (s > button::L_PRESSED && s <= button::M_PRESSED) cl = RGB(255, 65, 65);

		HBRUSH brush = CreateSolidBrush(cl);
		FillRect(hdc, rect, brush);
		DeleteObject(brush);
	}

	inline void onL_RELEASE(HWND hWnd, WPARAM wParam, LPARAM lParam) override {
		InvalidateRect(hWnd, nullptr, TRUE);
		PostMessageW(_hWndParent, WM_CLOSE, _buttonId, 1);
	}
};