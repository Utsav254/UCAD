#pragma once
#include "util/childwindow.hpp"
#include "util/UI.hpp"

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class toolBar : public childWindow
{
public:

	toolBar(int x, int y, int width, int height);
	~toolBar() = default;

	void createChildWindow() override;

	LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	void paint() override;

	inline const HWND getWindowHandle() const { return _hWnd; }

private:
	bool _dirty;
	ui _ui;
	ImVec4 _color;
};

