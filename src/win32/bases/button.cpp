#include "win32/bases/button.hpp"

button::button(HINSTANCE hInst,
	const int width, const int height,
	const int x, const int y,
	const int buttonId) :

	window(&wc, hInst, L"ButtonClass"),
	_hWndParent(nullptr),
	_width(width), _height(height),
	_x(x), _y(y),
	_buttonId(buttonId), _hWnd(nullptr),
	_s(NOT_ON)
{}

void button::createWindow(const bool showWindow, HWND parent)
{
	if (parent == nullptr) throw ERROR_FMT_M("button was given nullptr parent window");

	_hWndParent = parent;
	LPCWSTR className = getClassName();
	HINSTANCE hInst = getInstance();

	_hWnd = CreateWindowExW(
		0,
		className,
		L"Button",
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		_x, _y,
		_width, _height,
		parent, nullptr,
		hInst, this
	);
	if (!_hWnd) throw WINDOW_ERROR;
	if (showWindow) ShowWindow(_hWnd, SW_SHOWNORMAL);
}

LRESULT button::handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_CREATE:
		{
			onCreate(hWnd, wParam, lParam);
			break;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			RECT rect;
			GetClientRect(hWnd, &rect);

			onPaint(hdc, &rect, _s);

			EndPaint(hWnd, &ps);
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme = { 0 };
			tme.cbSize = sizeof(tme);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = hWnd;
			TrackMouseEvent(&tme);
			_s |= HOVER;
			onHover(hWnd, wParam, lParam);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			_s |= L_PRESSED;
			onL_PRESSED(hWnd, wParam, lParam);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			_s |= R_PRESSED;
			onR_PRESSED(hWnd, wParam, lParam);
			break;
		}
		case WM_MBUTTONDOWN:
		{
			_s |= M_PRESSED;
			onM_PRESSED(hWnd, wParam, lParam);
			break;
		}
		case WM_LBUTTONUP:
		{
			_s &= ~L_PRESSED;
			_s |= HOVER;
			onL_RELEASE(hWnd, wParam, lParam);
			break;
		}
		case WM_RBUTTONUP:
		{
			_s &= ~R_PRESSED;
			_s |= HOVER;
			onR_RELEASE(hWnd, wParam, lParam);
			break;
		}
		case WM_MBUTTONUP:
		{
			_s &= ~M_PRESSED;
			_s |= HOVER;
			onM_RELEASE(hWnd, wParam, lParam);
			break;
		}
		case WM_MOUSELEAVE:
		{
			_s = NOT_ON;
			onNOT_ON(hWnd, wParam, lParam);
			break;
		}
		case WM_DESTROY:
		{
			int i = getInstanceCount();
			if (i == -1) throw ERROR_FMT_M("Internal windowing frame work error ... WNDCLASSEX name not found in _registry of WNDLCASSEX manager");
			if (i == 1) PostQuitMessage(0);
			this->cleanUp();
			return 0;
		}
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

inline void button::onCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int radius = min(_width, _height) / 5;
	HRGN hRgn = CreateRoundRectRgn(0, 0, _width, _height, radius, radius);
	SetWindowRgn(hWnd, hRgn, TRUE);
}

inline void button::onHover(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(hWnd, nullptr, FALSE);
}

inline void button::onL_PRESSED(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(hWnd, nullptr, FALSE);
}

inline void button::onR_PRESSED(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(hWnd, nullptr, FALSE);
}

inline void button::onM_PRESSED(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(hWnd, nullptr, FALSE);
}

inline void button::onL_RELEASE(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(hWnd, nullptr, FALSE);
}

inline void button::onR_RELEASE(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(hWnd, nullptr, FALSE);
}

inline void button::onM_RELEASE(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(hWnd, nullptr, FALSE);
}

inline void button::onNOT_ON(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(hWnd, nullptr, FALSE);
}
