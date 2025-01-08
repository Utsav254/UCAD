#pragma once
#include "util/windowError.hpp"
#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class bindable
{
public:
	bindable(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context):
		_device(device), _context(context)
	{
		if ((!_device) || (!_context)) throw ERROR_FMT_M("Bindable recieved nullptr D3D object");
	}

	virtual ~bindable() = default;

	virtual void bind() = 0;
	virtual void unbind() = 0;

protected:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _context;
};
