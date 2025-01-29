#pragma once
#include "render.hpp" // Your render system from previous example

class CubeDemo {
public:
    CubeDemo(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
        : _registry(), _renderSystem(device, context), _cubeMesh(L"CubeEntityMesh")
    {
        createCubeMesh();
    }

    void render() {
        _renderSystem.updateMesh(_registry, _cubeMesh);
        _renderSystem.render();
    }

private:
    void createCubeMesh() {

        // Create vertices (8 corners of the cube)
        std::vector<entt::entity> vertices;
        vertices.reserve(8);

        // Front face vertices
        vertices.push_back(createVertex({ -1.0f, -1.0f, -1.0f }));  // 0: front bottom left
        vertices.push_back(createVertex({ 1.0f, -1.0f, -1.0f }));  // 1: front bottom right
        vertices.push_back(createVertex({ 1.0f,  1.0f, -1.0f }));  // 2: front top right
        vertices.push_back(createVertex({ -1.0f,  1.0f, -1.0f }));  // 3: front top left

        // Back face vertices
        vertices.push_back(createVertex({ -1.0f, -1.0f,  1.0f }));  // 4: back bottom left
        vertices.push_back(createVertex({ 1.0f, -1.0f,  1.0f }));  // 5: back bottom right
        vertices.push_back(createVertex({ 1.0f,  1.0f,  1.0f }));  // 6: back top right
        vertices.push_back(createVertex({ -1.0f,  1.0f,  1.0f }));  // 7: back top left

        // Create edges (12 edges of the cube)
        // Front face
        createEdge(vertices[0], vertices[1]);  // bottom
        createEdge(vertices[1], vertices[2]);  // right
        createEdge(vertices[2], vertices[3]);  // top
        createEdge(vertices[3], vertices[0]);  // left

        // Back face
        createEdge(vertices[4], vertices[5]);  // bottom
        createEdge(vertices[5], vertices[6]);  // right
        createEdge(vertices[6], vertices[7]);  // top
        createEdge(vertices[7], vertices[4]);  // left

        // Connecting edges
        createEdge(vertices[0], vertices[4]);  // bottom left
        createEdge(vertices[1], vertices[5]);  // bottom right
        createEdge(vertices[2], vertices[6]);  // top right
        createEdge(vertices[3], vertices[7]);  // top left

        // Create facets (6 faces of the cube)
        // Front face (facing -Z)
        createQuadFacet(
            vertices[0], vertices[1], vertices[2], vertices[3],
            dx::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f),
            dx::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f)
        );

        // Back face (facing +Z)
        createQuadFacet(
            vertices[5], vertices[4], vertices[7], vertices[6],
            dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
            dx::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f)
        );

        // Right face (facing +X)
        createQuadFacet(
            vertices[1], vertices[5], vertices[6], vertices[2],
            dx::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
            dx::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f),
            true
        );

        // Left face (facing -X)
        createQuadFacet(
            vertices[4], vertices[0], vertices[3], vertices[7],
            dx::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f),
            dx::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f)
        );

        // Top face (facing +Y)
        createQuadFacet(
            vertices[3], vertices[2], vertices[6], vertices[7],
            dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
            dx::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f)
        );

        // Bottom face (facing -Y)
        createQuadFacet(
            vertices[1], vertices[0], vertices[4], vertices[5],
            dx::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f),
            dx::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f)
        );

        _cubeMesh.isDirty = true;
    }

    entt::entity createVertex(const dx::XMFLOAT3& pos) {
        auto entity = _registry.create();
        _registry.emplace<posVertex>(entity, dx::XMLoadFloat3(&pos));
        return entity;
    }

    void createEdge(entt::entity start, entt::entity end) {
        auto edgeEntity = _registry.create();
        _registry.emplace<edge>(edgeEntity, start, end);
        _cubeMesh.addEdge(edgeEntity);
    }

    void createQuadFacet(
        entt::entity v1, entt::entity v2, entt::entity v3, entt::entity v4,
        dx::XMVECTOR normal, dx::XMVECTOR color, bool isSelected = false)
    {
        auto facetEntity = _registry.create();
        std::vector<entt::entity> vertices = { v1, v2, v3, v1, v3, v4 };  // Triangle list order
        auto& facetComponent = _registry.emplace<facet>(facetEntity, vertices);
        facetComponent.normal = normal;
        facetComponent.colour = color;
        facetComponent.isSelected = isSelected;
        _cubeMesh.addFacet(facetEntity);
    }

    entt::registry _registry;
    RenderSystem _renderSystem;
    mesh _cubeMesh;
};