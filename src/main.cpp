#include "util/win.h"
#include "MainWindow/mainWindow.hpp"
#include "util/error.hpp"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
) {
	try {
		if (!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
			throw ERROR_FMT_M("Unable to set high dpi awareness");

		window::startUp(hInstance);

		mainWindow wnd(1920, 1080, L"Main Window");
		wnd.createWindow(true);


		MSG windowMessage;
		BOOL windowMessageResult;
		while ((windowMessageResult = GetMessageW(&windowMessage, nullptr, 0, 0)) > 0) {
			TranslateMessage(&windowMessage);
			DispatchMessageW(&windowMessage);

			try {
				wnd.renderAll();
			}
			catch (const error& e) {
				e.display();
			}
		}

		window::shutDown();
		if (windowMessageResult == -1) return -1;
		else return (int)windowMessage.wParam;
	}
	catch (const error& e) {
		e.display();
		return EXIT_FAILURE;
	}
}


