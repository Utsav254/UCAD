#include "mesh.hpp"

posVertex::posVertex(dx::XMVECTOR pos):
    pos(pos), isSelected(false)
{
}

facet::facet(const std::vector<entt::entity>& vertices) :
    vertexEntities(vertices),
    normal(dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)),
    colour(dx::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)),
    isSelected(false)
{
}

void facet::addVertex(const entt::entity vertex)
{
    vertexEntities.push_back(vertex);
}

edge::edge(entt::entity start, entt::entity end) :
    startVertexIndex(start), endVertexIndex(end), isSelected(false)
{
}

mesh::mesh(LPCWSTR name) :
    name(name), isDirty(true)
{
}

void mesh::addEdge(entt::entity edge)
{
    edgesList.push_back(edge);
    isDirty = true;
}

void mesh::addFacet(entt::entity facet)
{
    facetList.push_back(facet);
    isDirty = true;
}