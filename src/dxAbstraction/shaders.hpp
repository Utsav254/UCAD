#pragma once
#include "dxAbstraction/bindable.hpp"
#include <type_traits>
#include <d3dcompiler.h>

template<typename typeOfShader>
class shader : public bindable
{
public:
    shader(
        ComPtr<ID3D11Device> device,
        ComPtr<ID3D11DeviceContext> context,
        LPCWSTR path) :
        bindable(device, context),
        _path(path)
    {
        HRESULT hr = S_FALSE;
        RUN_DX11(D3DReadFileToBlob(path, &_blob));

        if constexpr (std::is_same_v<typeOfShader, ID3D11VertexShader>) {
            RUN_DX11(_device->CreateVertexShader(
                _blob->GetBufferPointer(), _blob->GetBufferSize(),
                nullptr, &_shaderObject
            ));
        }
        else if constexpr (std::is_same_v<typeOfShader, ID3D11PixelShader>) {
            RUN_DX11(_device->CreatePixelShader(
                _blob->GetBufferPointer(), _blob->GetBufferSize(),
                nullptr, &_shaderObject
            ));
        }
        else if constexpr (std::is_same_v<typeOfShader, ID3D11GeometryShader>) {
            RUN_DX11(_device->CreateGeometryShader(
                _blob->GetBufferPointer(), _blob->GetBufferSize(),
                nullptr, &_shaderObject
            ));
        }
        else {
            static_assert(false, "Unknown shader type provided");
        }
    }

    ~shader() = default;

    void bind() override
    {
        if constexpr (std::is_same_v<typeOfShader, ID3D11VertexShader>) {
            _context->VSSetShader(_shaderObject.Get(), nullptr, 0);
        }
        else if constexpr (std::is_same_v<typeOfShader, ID3D11PixelShader>) {
            _context->PSSetShader(_shaderObject.Get(), nullptr, 0);
        }
        else if constexpr (std::is_same_v<typeOfShader, ID3D11GeometryShader>) {
            _context->GSSetShader(_shaderObject.Get(), nullptr, 0);
        }
        else {
            static_assert(false, "Unknown shader type being bound");
        }
    }

    void unbind() override
    {
        if constexpr (std::is_same_v<typeOfShader, ID3D11VertexShader>) {
            _context->VSSetShader(nullptr, nullptr, 0);
        }
        else if constexpr (std::is_same_v<typeOfShader, ID3D11PixelShader>) {
            _context->PSSetShader(nullptr, nullptr, 0);
        }
        else if constexpr (std::is_same_v<typeOfShader, ID3D11GeometryShader>) {
            _context->GSSetShader(nullptr, nullptr, 0);
        }
        else {
            static_assert(false, "Unknown shader type being unbound");
        }
    }

    inline ComPtr<ID3DBlob> getBlob() const { return _blob; }
    LPCWSTR getPath() const { return _path; }

private:
    ComPtr<typeOfShader> _shaderObject;
    LPCWSTR _path;
    ComPtr<ID3DBlob> _blob;
};
