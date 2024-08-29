#pragma once
#include <UCADWin.h>

// Encapsulating class for window creation and destruction ...
// Easily create windows by instantiating a this class ... clean up is automatically handled
// To handle multiple instances the message pump will need modification maybe multi threading the windows?
// To modify the behaviour overriding the member functions should work because of the message redirection system

class window {
public:
	window(const int width , const int height , LPCWSTR windowName) noexcept;
	~window();
	
private:
	//handling WndProc using static member function and a member function
	static LRESULT CALLBACK handleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static inline LRESULT CALLBACK handleMessageRedirect(HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam) noexcept;
	//WndProc or message handled for this window class
	LRESULT CALLBACK handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	//member variables
	int _width, _height;
	HWND _hWnd;

//----------------singleton windowClass automatic register and unregister--------------------------

private:
	class windowClass {
	public:
		static inline const LPCWSTR getName() noexcept;
		static inline const HINSTANCE getInstance() noexcept;
	private:
		windowClass() noexcept;
		~windowClass();
		//explicir delete on copies for this singleton 
		windowClass(const windowClass&) = delete;
		windowClass& operator= (const windowClass&) = delete;

		//member variables
		static constexpr const LPCWSTR _windowClassName = L"MainWindowClass";
		static windowClass _windowClass;
		HINSTANCE _hInst;
	};
};