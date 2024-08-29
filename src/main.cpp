#include "UCADWin.h"
#include "winClass.hpp"

int WINAPI WinMain (
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
) 
{
    LPCWSTR windowName = L"Main Window";
    window wnd(640 , 480 , windowName);

    MSG windowMessage;
    BOOL windowMessageResult;
    while ((windowMessageResult = GetMessage(&windowMessage, nullptr, 0, 0)) > 0) {
        TranslateMessage(&windowMessage);
        DispatchMessageW(&windowMessage);
    }

    if (windowMessageResult == -1) return -1;
    else return (int)windowMessage.wParam;
}

