#pragma once
#include "entt/entt.hpp"
#include "win.h"
#include "dxAbstraction/buffers.hpp"

#include <vector>
#include <string>
#include <DirectXMath.h>

namespace dx = DirectX;

struct posVertex
{
    dx::XMVECTOR pos;
    bool isSelected;

    posVertex(dx::XMVECTOR pos);
};

struct facet
{
    std::vector<entt::entity> vertexEntities;
    dx::XMVECTOR normal;
    dx::XMVECTOR colour;
    bool isSelected;

    facet(const std::vector<entt::entity>& vertices);
    void addVertex(const entt::entity vertex);
};

struct edge
{
    entt::entity startVertexIndex;
    entt::entity endVertexIndex;
    bool isSelected;

    edge(entt::entity start, entt::entity end);
};

struct mesh
{
    std::wstring name;
    std::vector<entt::entity> facetList;
    std::vector<entt::entity> edgesList;
    bool isDirty;

    void addEdge(entt::entity edge);
    void addFacet(entt::entity facet);

    mesh(LPCWSTR name);
};
