#pragma once
#include "win32/bases/window.hpp"

class button : public window {
public:

	static constexpr uint32_t NOT_ON = 0;
	static constexpr uint32_t HOVER = 1;
	static constexpr uint32_t L_PRESSED = 1 << 1;
	static constexpr uint32_t R_PRESSED = 1 << 2;
	static constexpr uint32_t M_PRESSED = 1 << 3;
	static constexpr uint32_t USER_DEF = 1 << 4;

public:

	button(HINSTANCE hInst,
		const int width, const int height,
		const int x, const int y,
		const int buttonId);

	virtual ~button() = default;

	void createWindow(const bool showWindow, HWND parent) override;

	LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	virtual inline void onPaint(HDC hdc, const RECT * const rect, const uint32_t _s) const = 0;

	virtual inline void onCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);

	virtual inline void onHover(HWND hWnd, WPARAM wParam, LPARAM lParam);

	virtual inline void onL_PRESSED(HWND hWnd, WPARAM wParam, LPARAM lParam);

	virtual inline void onR_PRESSED(HWND hWnd, WPARAM wParam, LPARAM lParam);

	virtual inline void onM_PRESSED(HWND hWnd, WPARAM wParam, LPARAM lParam);

	virtual inline void onL_RELEASE(HWND hWnd, WPARAM wParam, LPARAM lParam);

	virtual inline void onR_RELEASE(HWND hWnd, WPARAM wParam, LPARAM lParam);

	virtual inline void onM_RELEASE(HWND hWnd, WPARAM wParam, LPARAM lParam);

	virtual inline void onNOT_ON(HWND hWnd, WPARAM wParam, LPARAM lParam);

protected:

	HWND _hWndParent;
	int _width, _height;
	int _x, _y;
	const int _buttonId;
	HWND _hWnd;

	uint32_t _s;

private:

	static inline WNDCLASSEX wc =
	{
		.cbSize = sizeof(WNDCLASSEXW),
		.style = 0,
		.lpfnWndProc = DefWindowProcW,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = nullptr,
		.hIcon = nullptr,
		.hCursor = nullptr,
		.hbrBackground = nullptr,
		.lpszMenuName = nullptr,
		.lpszClassName = nullptr,
		.hIconSm = nullptr,
	};
};
