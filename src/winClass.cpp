#include "winClass.hpp"
#include "windowsMessageLog.hpp"

// 
// This is a c++ style abstraction for win32 where a window class encapsulates
// the creation and destruction of a window
// The Class encapsulated a singleton windowClass which is automatically registered and unregistered
// 
// The following are the function definitions for the single windowClass
//

//instantiate the singleton static member
window::windowClass window::windowClass::_windowClass;

//constructor for singleton windowClass
//registers a window class with the os and directs the WndProc to the setup handler
window::windowClass::windowClass() noexcept : _hInst(GetModuleHandleW(nullptr))
{   
    //settings for the window
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC; // change these as required
    wc.lpfnWndProc = handleMessageSetup; // point the class to the setup function which will redirect it later
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = getInstance();
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = getName();
    wc.hIcon = nullptr;
    RegisterClassExW(&wc);
}

//destructor will automatically unregister the class
window::windowClass::~windowClass() {
    UnregisterClass(getName() , getInstance());
}

inline const LPCWSTR window::windowClass::getName() noexcept {
    return _windowClassName;
}

inline const HINSTANCE window::windowClass::getInstance() noexcept {
    return _windowClass._hInst;
}

//
// The following are the function definitions for the over-arching window class
// 

//constructor creates a window of specified size and name
window::window(const int width, const int height, LPCWSTR windowName) noexcept : 
    _width(width) , _height(height)
{
    //adjust style as required
    const DWORD winStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_MAXIMIZEBOX;

    //adjust for client region and window size mismatch
    RECT tempR;
    tempR.left = 100;
    tempR.right = _width + tempR.left;
    tempR.top = 100;
    tempR.bottom = _height + tempR.top;
    if (AdjustWindowRect(&tempR, winStyle, false) == 0) {
        //throw exception;
        DebugBreak();
    }
    
    //create window
    _hWnd = CreateWindowExW(0, windowClass::getName(),
        windowName, winStyle,
        CW_USEDEFAULT, CW_USEDEFAULT, tempR.right - tempR.left, tempR.bottom - tempR.top,
        nullptr, nullptr, windowClass::getInstance(), this
    );

    if(_hWnd != nullptr) ShowWindow(_hWnd, SW_SHOWDEFAULT);
    else {
        //throw exception
        DebugBreak();
    }
}

//simple destructor to destroy the window instance
window::~window() {
    DestroyWindow(_hWnd);
}

// Window Message handling system
// The Window Class is registered to call the handeMessageSetup function
// This function checks for the initial CREATE message and extracts the instance of the window from the lparam
// Then it attaches the window instance to the window handle
// 
// It also redirects the WndProc message handler to the handleMessageRedirect which itself
// is an inline which redirects the function to member function to handle the messages
// 
// This system seems a little weird but it allows us to attach the windows instance to the 
// window handle and also redirect all messages to a member function

LRESULT CALLBACK window::handleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
    
    if (msg == WM_NCCREATE) {
        //use lParam has pointer to a CREATESTRACTW
        const CREATESTRUCTW * const tempCreateStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
        //safe to assume that this not a nullptr ?

        //fetch the lpCreateParams as this is the pointer to our window instance
        window* const tempWindow = static_cast<window*>(tempCreateStruct->lpCreateParams);

        //set the fetched window instance pointer to the USERDATA of the window handle
        SetWindowLongPtrW(hWnd , GWLP_USERDATA , reinterpret_cast<LONG_PTR>(tempWindow));
        //also redirect the WndProc function pointer to the redirector
        SetWindowLongPtrW(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&window::handleMessageRedirect));

        //forward this particular request to member function message handler
        return tempWindow->handleMessage(hWnd, msg, wParam, lParam);
    }
    //if not WM_NCCREATE let the default handler handle this message:
    return DefWindowProc(hWnd , msg , wParam , lParam);
}

// this inliner extracts the window class instance and calls the handle message function
inline LRESULT CALLBACK window::handleMessageRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
    //fetch window instance
    window* const tempWindow = reinterpret_cast<window*>(GetWindowLongPtr(hWnd , GWLP_USERDATA));
    //forward to our message handler
    return tempWindow->handleMessage(hWnd , msg , wParam , lParam);
}

//member function message handler
LRESULT CALLBACK window::handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {

    /*
    static windowsMessageLog wml;
    static std::wstring str;

    wml.get_str(str, msg, wParam, lParam);
    OutputDebugStringW((LPCWSTR)str.c_str());

    */

    switch (msg) {
    case WM_CLOSE:
        PostQuitMessage(1); // map WM_CLOSE to add a WM_QUIT to the queue
        return 1;
        break;
    case WM_KEYDOWN:
        if (wParam == 'F') SetWindowTextW(hWnd, L"Nice!");
        else SetWindowTextW(hWnd, L"Not Nice:(");
        break;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

