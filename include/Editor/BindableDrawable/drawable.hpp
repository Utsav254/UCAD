#pragma once
#include "Editor/BindableDrawable/buffers.hpp"

class drawable
{
public:
	drawable(ComPtr<ID3D11Device> device,
        ComPtr<ID3D11DeviceContext> context) :
        _device(device),
        _context(context)
    {}

    virtual ~drawable() = default;

    virtual void bindAll() = 0;
    virtual void draw() = 0;

protected:
    ComPtr<ID3D11Device> _device;
    ComPtr<ID3D11DeviceContext> _context;
};