#include "pch.hpp"

#include "window.hpp"
#include "error.hpp"

int WINAPI WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInst, _In_ LPSTR cmdLine, _In_ int ShowCmd)
{
	try {
		if (!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
			throw ERROR_FMT_M("Unable to set high dpi awareness");

		window wnd;

        MSG windowMessage;
        bool running = true;

        while (running) {
            if (PeekMessageW(&windowMessage, nullptr, 0, 0, PM_REMOVE)) {
                if (windowMessage.message == WM_QUIT) {
                    running = false; // Exit the loop if we receive a quit message
                }
                else {
                    TranslateMessage(&windowMessage);
                    DispatchMessageW(&windowMessage);
                }
            }
            else {
                try {
                    wnd.paint();
                }
                catch (const error& e) {
                    e.display();
                }
            }
        }
		return (int)windowMessage.wParam;
	}
	catch (const error& e) {
		e.display();
		return EXIT_FAILURE;
	}
}


