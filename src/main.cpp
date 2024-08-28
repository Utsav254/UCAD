#include <Windows.h>

LRESULT CALLBACK WndProc(HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam);

int WINAPI WinMain (
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
) {

    LPCWSTR className = L"Main Class";
    LPCWSTR windowName = L"Main Window";

    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = className;
    wc.hIcon = nullptr;

    RegisterClassEx(&wc);

    HWND hWnd = CreateWindowEx (
        0,
        className ,
        windowName ,
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU ,
        200 , 200 ,
        640 , 480 ,
        nullptr , nullptr ,
        hInstance ,
        nullptr
    );

    ShowWindow(hWnd, SW_SHOW);

    MSG windowMessage;
    BOOL windowMessageResult;
    while ((windowMessageResult = GetMessage(&windowMessage, nullptr, 0, 0)) > 0) {
        TranslateMessage(&windowMessage);
        DispatchMessage(&windowMessage);
    }

    if (windowMessageResult == -1) return -1;
    else return windowMessage.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        PostQuitMessage(1); // map WM_CLOSE to add a WM_QUIT to the queue
        break;
    }
    return DefWindowProc(hWnd, msg , wParam, lParam);
}
