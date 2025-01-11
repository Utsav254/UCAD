#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <memory>
#include <vector>
#include "Editor/BindableDrawable/drawable.hpp"
#include "Editor/BindableDrawable/vertex.hpp"

#undef max

class model : public drawable {
public:
	model(ComPtr<ID3D11Device> device,
		ComPtr<ID3D11DeviceContext> context) :
		drawable(device, context),
		_importer(),
		_scene(_importer.ReadFile("test/rocket.obj", 0))
	{
		std::string errmsg = _importer.GetErrorString();
		if (!_scene) throw ERROR_FMT_M("Unable to create aiScene from given CAD file");

		for (unsigned int m = 0; m < _scene->mNumMeshes; ++m) {
			aiMesh* mesh = _scene->mMeshes[m];

			float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;
			float maxX = -FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;

			for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
				aiVector3D vertex = mesh->mVertices[v];
				minX = std::min(minX, vertex.x);
				minY = std::min(minY, vertex.y);
				minZ = std::min(minZ, vertex.z);
				maxX = std::max(maxX, vertex.x);
				maxY = std::max(maxY, vertex.y);
				maxZ = std::max(maxZ, vertex.z);
			}
			float centerX = (minX + maxX) / 2.0f;
			float centerY = (minY + maxY) / 2.0f;
			float centerZ = (minZ + maxZ) / 2.0f;

			for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
				aiVector3D vertex = mesh->mVertices[v];
				aiVector3D normal = mesh->mNormals ? mesh->mNormals[v] : aiVector3D(0.0f, 0.0f, 1.0f); // Default normal if none found

				_vertices.emplace_back(vertex.x - centerX, vertex.y - centerY, vertex.z - centerZ, normal.x, normal.y, normal.z);
			}

			for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
				aiFace& face = mesh->mFaces[f];
				for (unsigned int i = 0; i < face.mNumIndices; ++i) {
					_indices.emplace_back(face.mIndices[i]);
				}
			}
		}

		if (!_vertexShader)
			_vertexShader = std::make_shared<shader<ID3D11VertexShader>>(device, context, L"exe/x64/Debug/VertexShader.cso");
		if (!_pixelShader)
			_pixelShader = std::make_shared<shader<ID3D11PixelShader>>(device, context, L"exe/x64/Debug/PixelShader.cso");

		_vb = std::make_unique<vertexBuffer<vertex>>(device, context, _vertices.data(), _vertices.size(), _vertexShader.get(), D3D11_USAGE_DEFAULT, 0, 0);
		_ib = std::make_unique<indexBuffer<unsigned int>>(device, context, _indices.data(), _indices.size(), D3D11_USAGE_DEFAULT, 0, 0);
	}

	~model() = default;

	void bindAll() override
	{
		_vertexShader->bind();
		_pixelShader->bind();
		_vb->bind();
		_ib->bind();
	}

	void draw() override
	{
		_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_context->DrawIndexed((UINT)_indices.size(), 0, 0);
	}

private:
	Assimp::Importer _importer;
	const aiScene* _scene;

	std::vector<vertex> _vertices;
	std::vector<unsigned int> _indices;

	static inline std::shared_ptr<shader<ID3D11VertexShader>>_vertexShader;
	static inline std::shared_ptr<shader<ID3D11PixelShader>> _pixelShader;

	std::unique_ptr<vertexBuffer<vertex>> _vb;
	std::unique_ptr<indexBuffer<unsigned int>> _ib;
};
