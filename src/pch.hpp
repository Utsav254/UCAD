#pragma once

//microsoft
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <d3d11.h>
#include<DirectXMath.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

//vendor
#include "entt/entt.hpp"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// global definitions
namespace dx = DirectX;
using Microsoft::WRL::ComPtr;