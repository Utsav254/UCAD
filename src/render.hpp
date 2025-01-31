#pragma once
#include "pch.hpp"

#include "buffers.hpp"
#include "shaders.hpp"
#include "mesh.hpp"

#include <memory>
#include <vector>

struct RenderVertex {
	dx::XMFLOAT3 position;
	dx::XMFLOAT3 normal;
	dx::XMFLOAT4 color;
	float isSelected;

	static D3D11_INPUT_ELEMENT_DESC* getDesc(UINT* size) {
		static D3D11_INPUT_ELEMENT_DESC desc[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"SELECTED", 0, DXGI_FORMAT_R32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		*size = _countof(desc);
		return desc;
	}
};

struct LineVertex {
	dx::XMFLOAT3 position;
	dx::XMFLOAT4 color;
	float isSelected;

	static D3D11_INPUT_ELEMENT_DESC* getDesc(UINT* size) {
		static D3D11_INPUT_ELEMENT_DESC desc[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"SELECTED", 0, DXGI_FORMAT_R32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		*size = _countof(desc);
		return desc;
	}
};

struct ViewProjectionBuffer {
	dx::XMMATRIX viewProjection;
};

class RenderSystem {
public:
	RenderSystem(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
		: _device(device), _context(context) {
		// Initialize shaders
		_facetVS = std::make_unique<shader<ID3D11VertexShader>>(device, context, L"exe/x64/Debug/facet_vs.cso");
		_facetPS = std::make_unique<shader<ID3D11PixelShader>>(device, context, L"exe/x64/Debug/facet_ps.cso");
		_edgeVS = std::make_unique<shader<ID3D11VertexShader>>(device, context, L"exe/x64/Debug/edge_vs.cso");
		_edgePS = std::make_unique<shader<ID3D11PixelShader>>(device, context, L"exe/x64/Debug/edge_ps.cso");

		HRESULT hr = S_FALSE;
		D3D11_RASTERIZER_DESC rastDesc = {};
		rastDesc.FillMode = D3D11_FILL_SOLID;
		rastDesc.CullMode = D3D11_CULL_BACK;
		rastDesc.FrontCounterClockwise = true;
		rastDesc.DepthClipEnable = true;
		rastDesc.AntialiasedLineEnable = true;
		ComPtr<ID3D11RasterizerState> rastState;
		RUN_DX11(device->CreateRasterizerState(&rastDesc, &rastState));
		context->RSSetState(rastState.Get());
	}

	void updateMesh(entt::registry& registry, mesh& currentMesh) {
		if (!currentMesh.isDirty) return;

		std::vector<RenderVertex> facetVertices;
		std::vector<LineVertex> edgeVertices;

		// Process facets
		for (const auto& facetEntity : currentMesh.facetList) {
			const auto& facetComponent = registry.get<facet>(facetEntity);

			for (const auto& vertexEntity : facetComponent.vertexEntities) {
				const auto& vertex = registry.get<posVertex>(vertexEntity);

				dx::XMFLOAT3 pos;
				dx::XMStoreFloat3(&pos, vertex.pos);

				dx::XMFLOAT3 normal;
				dx::XMStoreFloat3(&normal, facetComponent.normal);

				dx::XMFLOAT4 color;
				dx::XMStoreFloat4(&color, facetComponent.colour);

				facetVertices.push_back({
					pos, normal, color,
					facetComponent.isSelected ? 1.0f : 0.0f
					});
			}
		}

		for (const auto& edgeEntity : currentMesh.edgesList) {
			const auto& edgeComponent = registry.get<edge>(edgeEntity);

			const auto& startVertex = registry.get<posVertex>(edgeComponent.startVertexIndex);
			const auto& endVertex = registry.get<posVertex>(edgeComponent.endVertexIndex);

			dx::XMFLOAT3 startPos, endPos;
			dx::XMStoreFloat3(&startPos, startVertex.pos);
			dx::XMStoreFloat3(&endPos, endVertex.pos);

			dx::XMFLOAT4 edgeColor = { 0.0f, 0.0f, 0.0f, 0.0f };

			edgeVertices.push_back({
				startPos, edgeColor,
				edgeComponent.isSelected ? 1.0f : 0.0f
				});
			edgeVertices.push_back({
				endPos, edgeColor,
				edgeComponent.isSelected ? 1.0f : 0.0f
				});
		}

		// Update or create vertex buffers
		if (!_facetBuffer && facetVertices.size() > 0) {
			_facetBuffer = std::make_unique<vertexBuffer<RenderVertex>>(
				_device, _context,
				facetVertices.data(), facetVertices.size(),
				*_facetVS,
				D3D11_USAGE_DEFAULT
			);
		}

		if (!_edgeBuffer && edgeVertices.size() > 0) {
			_edgeBuffer = std::make_unique<vertexBuffer<LineVertex>>(
				_device, _context,
				edgeVertices.data(), edgeVertices.size(),
				*_edgeVS,
				D3D11_USAGE_DEFAULT
			);
		}

		currentMesh.isDirty = false;
	}

	void render() {
		// Render facets
		if (_facetBuffer) {
			_facetVS->bind();
			_facetPS->bind();
			_facetBuffer->bind();

			_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			_context->Draw(static_cast<UINT>(_facetBuffer->getElementCount()), 0);
		}

		// Render edges
		if (_edgeBuffer) {
			_edgeVS->bind();
			_edgePS->bind();

			_edgeBuffer->bind();

			_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			_context->Draw(static_cast<UINT>(_edgeBuffer->getElementCount()), 0);
		}
	}

private:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _context;

	std::unique_ptr<shader<ID3D11VertexShader>> _facetVS;
	std::unique_ptr<shader<ID3D11PixelShader>> _facetPS;
	std::unique_ptr<shader<ID3D11VertexShader>> _edgeVS;
	std::unique_ptr<shader<ID3D11PixelShader>> _edgePS;

	std::unique_ptr<vertexBuffer<RenderVertex>> _facetBuffer;
	std::unique_ptr<vertexBuffer<LineVertex>> _edgeBuffer;
};