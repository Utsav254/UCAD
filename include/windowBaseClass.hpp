#pragma once
#include <win.h>
#include <type_traits>
#include <atomic>
#include "util/error.hpp"

#define WINDOW_ERROR windowError(__LINE__ , L"" __FILE__ , L"Window Error", GetLastError())
#define WINDOW_ERROR_HR(hr) windowError(__LINE__ , L"" __FILE__ , L"Window Error", hr)

class windowError : public error
{
public:
	windowError(const int line, LPCWSTR fileName, LPCWSTR message, HRESULT hr) :
		error(line, fileName, message), _hr(hr)
	{
		LPWSTR errorMsg = nullptr;
		FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, hr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
			(LPWSTR)&errorMsg, 0, nullptr
		);
		_whatBuffer += errorMsg ? std::wstring(errorMsg) : L"Unknown error";
		if (errorMsg) LocalFree(errorMsg);
	}

private:
	HRESULT _hr;
};

template <typename T>
concept hasDefineWindowClass = requires {
	{ T::defineWindowClass() } -> std::same_as<WNDCLASSEXW*>;
};

template <typename T>
concept hasHandleMessage = requires(T & obj, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	{ (obj.handleMessage(hWnd, msg, wParam, lParam)) } -> std::same_as<LRESULT>;
};

template <typename T>
concept hasClassName = requires {
	{ T::className } -> std::convertible_to<LPCWSTR>;
};

template<typename T>
concept hasGetInstance = requires() {
	{ (T::getInstance()) } -> std::convertible_to<const HINSTANCE>;
};

template<typename T>
concept hasGetHandle = requires(const T & obj) {
	{ (obj.getHandle()) } -> std::convertible_to<const HWND>;
};

template<typename Derived>
class window {
private:

	static_assert(std::is_class_v<Derived>, "Derived must be a class type");

protected:

	static inline std::atomic<int> _instanceCount{ 0 };
	static inline bool _isRegistered = false;

	window()
	{
		static_assert(hasHandleMessage<Derived>,
		"Derived must implement the LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) method");
		
		static_assert(hasDefineWindowClass<Derived>,
			"Derived must implement the static WNDCLASSEXW* defineWindowClass() method");

		static_assert(hasGetInstance<Derived>,
			"Derived must implement the (optional,reccomended: inline) static const HINSTANCE getInstance() method");

		static_assert(hasGetHandle<Derived>,
			"Derived must implement the (optional,reccomended: inline) const HWND getHandle() const method");

		static_assert(hasClassName<Derived>,
			"Derived must have public: static (optional: constexpr) LPCWSTR className member variable which is used as the registered WNDCLASSEX name");

		if (!_isRegistered) {
			WNDCLASSEXW* wc = Derived::defineWindowClass();
			wc->lpfnWndProc = handleMessageSetup;
			wc->lpszClassName = Derived::className;
			if(RegisterClassExW(wc) == 0) throw WINDOW_ERROR;
			_isRegistered = true;
		}
		_instanceCount++;
	}

	~window() noexcept
	{
		_instanceCount--;
		if (_instanceCount.load() == 0) {
			UnregisterClassW(Derived::className, Derived::getInstance());
		}
	}

private:
	static LRESULT CALLBACK handleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		if (msg == WM_NCCREATE) {
			const CREATESTRUCTW* const tempCreateStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
			Derived* const tempWindow = static_cast<Derived*>(tempCreateStruct->lpCreateParams);

			SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(tempWindow));
			SetWindowLongPtrW(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&window::handleMessageRedirect));
			return tempWindow->handleMessage(hWnd, msg, wParam, lParam);
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	static inline LRESULT CALLBACK handleMessageRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
		Derived* const tempWindow = reinterpret_cast<Derived*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		return tempWindow->handleMessage(hWnd, msg, wParam, lParam);
	}
};