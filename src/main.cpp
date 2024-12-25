#include "win.h"
#include "winClass.hpp"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
) {
	try {
		mainWindow wnd(640, 480, L"Main Window", hInstance);

		MSG windowMessage;
		BOOL windowMessageResult;
		while ((windowMessageResult = GetMessageW(&windowMessage, nullptr, 0, 0)) > 0) {
			TranslateMessage(&windowMessage);
			DispatchMessageW(&windowMessage);
		}

		if (windowMessageResult == -1) return -1;
		else return (int)windowMessage.wParam;
	}
	catch (const error& e) {
		e.display();
		return EXIT_FAILURE;
	}
}


