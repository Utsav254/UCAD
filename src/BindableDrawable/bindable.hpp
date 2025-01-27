#pragma once
#include "error.hpp"
#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class bindable
{
public:
	bindable(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context):
		_device(device), _context(context)
	{
	}

	virtual ~bindable() = default;

	virtual void bind() = 0;
	virtual void unbind() = 0;

protected:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _context;
};
