//#pragma once
//#include "Editor/BindableDrawable/drawable.hpp"
//#include "Editor/BindableDrawable/vertex.hpp"
//#include <array>
//
//class cube : public drawable{
//public:
//	cube(ComPtr<ID3D11Device> device,
//		ComPtr<ID3D11DeviceContext> context):
//		drawable(device, context),
//		_vertices(
//			{
//				vertex(-0.5f, -0.5f, -0.5f, 255,   0,   0, 255),
//				vertex( 0.5f, -0.5f, -0.5f,   0, 255,   0, 255),
//				vertex(-0.5f,  0.5f, -0.5f,   0,   0, 255, 255),
//				vertex( 0.5f,  0.5f, -0.5f,   0, 255, 255, 255),
//				vertex(-0.5f, -0.5f,  0.5f, 255,   0, 255, 255),
//				vertex( 0.5f, -0.5f,  0.5f, 255, 255,   0, 255),
//				vertex(-0.5f,  0.5f,  0.5f, 127, 127, 127, 255),
//				vertex( 0.5f,  0.5f,  0.5f,   0, 127, 127, 255),
//			}),
//			_indices(
//			{
//				0, 2, 1, 2, 3, 1,
//				1, 3, 5, 3, 7, 5,
//				2, 6, 3, 3, 6, 7,
//				4, 5, 7, 4, 7, 6,
//				0, 4, 2, 2, 4, 6,
//				0, 1, 4, 1, 5, 4,
//			})
//	{
//		if(!_vertexShader)
//			_vertexShader = std::make_shared<shader<ID3D11VertexShader>>(device, context, L"exe/x64/Debug/VertexShader.cso");
//		if(!_pixelShader)
//			_pixelShader = std::make_shared<shader<ID3D11PixelShader>>(device, context, L"exe/x64/Debug/PixelShader.cso");
//
//		_vb = std::make_unique<vertexBuffer<vertex>>(device, context, _vertices.data(), 8, _vertexShader.get(), D3D11_USAGE_DEFAULT, 0, 0);
//		_ib = std::make_unique<indexBuffer<unsigned int>>(device, context, _indices.data(), 36, D3D11_USAGE_DEFAULT, 0, 0);
//	}
//
//	~cube() = default;
//
//	void bindAll() override
//	{
//		_vertexShader->bind();
//		_pixelShader->bind();
//		_vb->bind();
//		_ib->bind();
//	}
//
//	void draw() override
//	{
//		_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		_context->DrawIndexed(36, 0, 0);
//	}
//
//private:
//	const std::array<vertex, 8> _vertices;
//	const std::array<unsigned int, 36> _indices;
//
//	// shared resources
//	static inline std::shared_ptr<shader<ID3D11VertexShader>>_vertexShader;
//	static inline std::shared_ptr<shader<ID3D11PixelShader>> _pixelShader;
//
//	// unique resources to each instance
//	std::unique_ptr<vertexBuffer<vertex>> _vb;
//	std::unique_ptr<indexBuffer<unsigned int>> _ib;
//};
