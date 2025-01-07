#pragma once
#include "util/win.h"
#include "util/windowError.hpp"
#include <mutex>
#include <unordered_map>
#include <string>
#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class window {
public:

	window(WNDCLASSEXW* wc);
	virtual ~window() noexcept;

	static void startUp(HINSTANCE hInst) noexcept;
	static void shutDown() noexcept;

	virtual void destroyWindow() noexcept final;

	virtual void createWindow(bool showWindow = true, HWND hWndParent = nullptr,
		ComPtr<ID3D11Device> device = nullptr, ComPtr<ID3D11DeviceContext> context = nullptr) = 0;

	virtual void paint() = 0;

	virtual LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;

public:
	const int getInstanceCount() const noexcept;
	inline LPCWSTR getClassName() const { return _className; }

protected:
	static inline HINSTANCE _hInst = nullptr;
	LPCWSTR _className;

private:
	static inline std::unordered_map<std::wstring, int> _registry;
	static inline std::mutex _mtx;

	static LRESULT CALLBACK handleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static inline LRESULT CALLBACK handleMessageRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
};