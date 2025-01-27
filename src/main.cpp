#include "win.h"
#include "window.hpp"
#include "error.hpp"

int WINAPI WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInst, _In_ LPSTR cmdLine, _In_ int ShowCmd)
{
	try {
		if (!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
			throw ERROR_FMT_M("Unable to set high dpi awareness");

		window wnd;

		MSG windowMessage;
		BOOL windowMessageResult;
		while ((windowMessageResult = GetMessageW(&windowMessage, nullptr, 0, 0)) > 0) {
			TranslateMessage(&windowMessage);
			DispatchMessageW(&windowMessage);

			try {
				wnd.paint();
			}
			catch (const error& e) {
				e.display();
			}
		}

		if (windowMessageResult == -1) return -1;
		else return (int)windowMessage.wParam;
	}
	catch (const error& e) {
		e.display();
		return EXIT_FAILURE;
	}
}


