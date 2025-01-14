#include "util/window.hpp"

window::window(WNDCLASSEXW* wc)
{
	std::lock_guard<std::mutex> lock(_mtx);

#ifdef _DEBUG
	if (_hInst == nullptr) __debugbreak();
#endif //_DEBUG

	_className = wc->lpszClassName;
	std::wstring classNameWString(wc->lpszClassName);
	auto it = _registry.find(classNameWString);

	if (it == _registry.end()) {
		wc->lpfnWndProc = handleMessageSetup;
		wc->hInstance = _hInst;

		if (RegisterClassExW(wc) == 0)
			throw WINDOW_ERROR;

		_registry.insert({ classNameWString, 1 });
	}
	else {
		it->second++;
	}
}

window::~window() noexcept
{
	std::lock_guard<std::mutex> lock(_mtx);

	std::wstring classNameWString(_className);
	auto it = _registry.find(classNameWString);

	if (it != _registry.end()) {
		if(it->second > 0) it->second--;
	}
}

void window::startUp(HINSTANCE hInst) noexcept
{
	std::lock_guard<std::mutex> lock(_mtx);
	_hInst = hInst;
}

void window::shutDown() noexcept
{
	std::lock_guard<std::mutex> lock(_mtx);
	for (std::unordered_map<std::wstring, int>::value_type& element : _registry) {	
		UnregisterClassW(element.first.c_str(), GetModuleHandleW(nullptr));
	}
}

void window::destroyWindow() noexcept
{
	std::lock_guard<std::mutex> lock(_mtx);

	std::wstring classNameWString(_className);
	auto it = _registry.find(classNameWString);

	if (it != _registry.end()) {
		if(it->second >0) it->second--;
	}

#ifdef _DEBUG
	else __debugbreak();
#endif // _DEBUG
}

const int window::getInstanceCount() const noexcept
{
	std::lock_guard <std:: mutex> lock(_mtx);
	auto it = _registry.find(_className);

	if (it != _registry.end()) return it->second;
	else return -1;
}

LRESULT CALLBACK window::handleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (msg == WM_NCCREATE) {
		const CREATESTRUCTW* const tempCreateStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
		window* const tempWindow = static_cast<window*>(tempCreateStruct->lpCreateParams);

		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(tempWindow));
		SetWindowLongPtrW(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&window::handleMessageRedirect));
		return tempWindow->handleMessage(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

inline LRESULT CALLBACK window::handleMessageRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	window* const tempWindow = reinterpret_cast<window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return tempWindow->handleMessage(hWnd, msg, wParam, lParam);
}
