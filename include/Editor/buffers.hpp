#pragma once
#include "Editor/bindable.hpp"
#include <type_traits>

template<typename T>
class vertexBuffer : public bindable {
public:
    vertexBuffer(
        ComPtr<ID3D11Device> device,
        ComPtr<ID3D11DeviceContext> context,
        const T* vertices,
        std::size_t count,
        D3D11_USAGE usage = D3D11_USAGE_DEFAULT,
        UINT cpuAccessFlags = 0,
        UINT miscFlags = 0
    ) : bindable(device, context), _elementCount(count)
    {
        if (!vertices || count == 0) {
            throw ERROR_FMT_M("Invalid Vertex Data content or size");
        }

        const D3D11_BUFFER_DESC bufferDesc = {
            .ByteWidth = static_cast<UINT>(count * sizeof(T)),
            .Usage = usage,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = cpuAccessFlags,
            .MiscFlags = miscFlags,
            .StructureByteStride = sizeof(T)
        };

        const D3D11_SUBRESOURCE_DATA subResData = { .pSysMem = vertices };
        RUN_DX11(_device->CreateBuffer(&bufferDesc, &subResData, &_vertexBuffer));
    }

    void bind() override {
        const UINT stride = sizeof(T);
        const UINT offset = 0;
        _context->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
    }

    void unbind() override {
        UINT stride = 0;
        UINT offset = 0;
        ID3D11Buffer* nullBuffer = nullptr;
        _context->IASetVertexBuffers(0, 1, &nullBuffer, &stride, &offset);
    }

    std::size_t getElementCount() const { return _elementCount; }

private:
    ComPtr<ID3D11Buffer> _vertexBuffer;
    std::size_t _elementCount;
};

template<typename T>
class indexBuffer : public bindable {
    static_assert(
        std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t>,
        "Index buffer type must be uint16_t or uint32_t"
        );

public:
    indexBuffer(
        ComPtr<ID3D11Device> device,
        ComPtr<ID3D11DeviceContext> context,
        const T* indices,
        std::size_t count,
        D3D11_USAGE usage = D3D11_USAGE_DEFAULT,
        UINT cpuAccessFlags = 0,
        UINT miscFlags = 0
    ) : bindable(device, context), _elementCount(count)
    {
        if (!indices || count == 0) {
            throw ERROR_FMT_M("Invalid Index Data Content or size");
        }

        const D3D11_BUFFER_DESC bufferDesc = {
            .ByteWidth = static_cast<UINT>(count * sizeof(T)),
            .Usage = usage,
            .BindFlags = D3D11_BIND_INDEX_BUFFER,
            .CPUAccessFlags = cpuAccessFlags,
            .MiscFlags = miscFlags,
            .StructureByteStride = sizeof(T)
        };

        const D3D11_SUBRESOURCE_DATA subResData = { .pSysMem = indices };
        RUN_DX11(_device->CreateBuffer(&bufferDesc, &subResData, &_indexBuffer));
    }

    void bind() override {
        constexpr DXGI_FORMAT format = sizeof(T) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        _context->IASetIndexBuffer(_indexBuffer.Get(), format, 0);
    }

    void unbind() override {
        _context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    }

    std::size_t getElementCount() const { return _elementCount; }

private:
    ComPtr<ID3D11Buffer> _indexBuffer;
    std::size_t _elementCount;
};

template<typename T, UINT shaderType>
class constantBuffer : public bindable
{
public:
    static constexpr UINT MAX_BUFFER_SLOTS = 14;

    enum shaderTypeEnum {
        VS = 0,
        PS = 1
    };

    constantBuffer(
        ComPtr<ID3D11Device> device,
        ComPtr<ID3D11DeviceContext> context,
        const T* data,
        std::size_t count,
        UINT slot = 0,
        D3D11_USAGE usage = D3D11_USAGE_DYNAMIC,
        UINT cpuAccessFlags = D3D11_CPU_ACCESS_WRITE,
        UINT miscFlags = 0
    ) :
        bindable(device, context),
        _slot(slot),
        _elementCount(count)
    {
        if (!data || count == 0) {
            throw ERROR_FMT_M("Invalid Constant Data or size");
        }

        if (slot >= MAX_BUFFER_SLOTS) {
            throw ERROR_FMT_M("Exceeds maximum buffer slots available");
        }

        const D3D11_BUFFER_DESC bufferDesc = {
            .ByteWidth = static_cast<UINT>(count * sizeof(T)),
            .Usage = usage,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = cpuAccessFlags,
            .MiscFlags = miscFlags,
            .StructureByteStride = sizeof(T)
        };

        const D3D11_SUBRESOURCE_DATA subResData = { .pSysMem = data };
        RUN_DX11(_device->CreateBuffer(&bufferDesc, &subResData, &_constantBuffer));
    }

    void bind() override
    {
        if constexpr (shaderType == VS) {
            _context->VSSetConstantBuffers(_slot, 1, _constantBuffer.GetAddressOf());
        }
        else if constexpr (shaderType == PS) {
            _context->PSSetConstantBuffers(_slot, 1, _constantBuffer.GetAddressOf());
        }
        else {
            static_assert(true, "Unsupported shader type");
        }
    }

    void unbind() override
    {
        ID3D11Buffer* nullBuffer = nullptr;
        if constexpr (shaderType == VS) {
            _context->VSSetConstantBuffers(_slot, 1, &nullBuffer);
        }
        else if constexpr (shaderType == PS) {
            _context->PSSetConstantBuffers(_slot, 1, &nullBuffer);
        }
    }

    void update(const void* data, size_t size)
    {
#ifdef _DEBUG
        if (!data || size == 0 || size > _bufferSize) {
            throw std::invalid_argument("Invalid update data or size");
        }
#endif

        D3D11_MAPPED_SUBRESOURCE mappedData;
        RUN_DX11(_context->Map(_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
        memcpy(mappedData.pData, data, size);
        _context->Unmap(_constantBuffer.Get(), 0);
    }

private:
    ComPtr<ID3D11Buffer> _constantBuffer;
    UINT _slot;
    size_t _elementCount;
};