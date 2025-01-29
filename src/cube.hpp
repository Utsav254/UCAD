#pragma once
#include "dxAbstraction/buffers.hpp"
#include <array>
#include <DirectXMath.h>
namespace dx = DirectX;

struct vertex {
    float x, y, z;
    unsigned char r, g, b, a;

    vertex(float px, float py, float pz, unsigned char pr, unsigned char pg, unsigned char pb, unsigned char pa)
        : x(px), y(py), z(pz), r(pr), g(pg), b(pb), a(pa) {
    }

    static D3D11_INPUT_ELEMENT_DESC* getDesc(UINT *size) {
        static D3D11_INPUT_ELEMENT_DESC inElDesc[] =
        {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        *size = std::size(inElDesc);
        return inElDesc;
    }
    
};

class cube {
public:
    cube(ComPtr<ID3D11Device> device,
        ComPtr<ID3D11DeviceContext> context):
        _device(device),
        _context(context),
        _vertices(
            {
                vertex(-0.5f, -0.5f, -0.5f, 255, 255, 255, 255),
                vertex( 0.5f, -0.5f, -0.5f, 255, 255, 255, 255),
                vertex(-0.5f,  0.5f, -0.5f, 255, 255, 255, 255),
                vertex( 0.5f,  0.5f, -0.5f, 255, 255, 255, 255),
                vertex(-0.5f, -0.5f,  0.5f, 255,   0,   0, 255),
                vertex( 0.5f, -0.5f,  0.5f, 255,   0,   0, 255),
                vertex(-0.5f,  0.5f,  0.5f, 255,   0,   0, 255),
                vertex( 0.5f,  0.5f,  0.5f, 255,   0,   0, 255),
            }),
            _indices(
            {
                0, 2, 1, 2, 3, 1,
                1, 3, 5, 3, 7, 5,
                2, 6, 3, 3, 6, 7,
                4, 5, 7, 4, 7, 6,
                0, 4, 2, 2, 4, 6,
                0, 1, 4, 1, 5, 4,
            }),
            _vertexShader(device, context, L"exe/x64/Debug/VertexShader.cso"),
            _pixelShader(device, context, L"exe/x64/Debug/PixelShader.cso"),
            _vb(device, context, _vertices.data(), 8, _vertexShader, D3D11_USAGE_DEFAULT, 0, 0),
            _ib(device, context, _indices.data(), 36, D3D11_USAGE_DEFAULT, 0, 0)
    {
    }

    ~cube() = default;

    void bindAll()
    {
        _vertexShader.bind();
        _pixelShader.bind();
        _vb.bind();
        _ib.bind();
    }

    void draw()
    {
        _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        _context->DrawIndexed(36, 0, 0);
    }

private:
    ComPtr<ID3D11Device> _device;
    ComPtr<ID3D11DeviceContext> _context;

    const std::array<vertex, 8> _vertices;
    const std::array<unsigned short, 36> _indices;

    shader<ID3D11VertexShader> _vertexShader;
    shader<ID3D11PixelShader> _pixelShader;
    vertexBuffer<vertex> _vb;
    indexBuffer<unsigned short> _ib;
};
