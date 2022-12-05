#pragma once

#include "ranges.h"

#include <cstdlib>
#include <vector>
#include <deque>

namespace graph
{

    using VertexId = size_t;
    using EdgeId = size_t;

    // Ребро пути(откуда, куда, вес пути)
    template <typename Weight>
    struct Edge
    {
        VertexId from;
        VertexId to;
        Weight weight;
    };

    // Класс, реализующий взвешенный ориентированный граф
    template <typename Weight>
    class DirectedWeightedGraph
    {
    private:
        using IncidenceList = std::deque<EdgeId>;
        using IncidentEdgesRange = ranges::Range<typename IncidenceList::const_iterator>;

    public:
        DirectedWeightedGraph() = default;
        explicit DirectedWeightedGraph(size_t vertex_count);

        // Добавляет ребро(путь)
        EdgeId AddEdge(const Edge<Weight> &edge);

        // Возвращает количество вершин(остановок) графа
        size_t GetVertexCount() const;

        // Возвращает количество рёбер(путей) графа
        size_t GetEdgeCount() const;

        // Возвращает ссылку на ребро(путь)
        const Edge<Weight> &GetEdge(EdgeId edge_id) const;

        // Возвращает количество рёбер(путей), выходящих из вершины X
        IncidentEdgesRange GetIncidentEdges(VertexId vertex) const;

    protected:
        void AddVertexCount(size_t count);

    private:
        std::vector<Edge<Weight>> edges_;
        std::vector<IncidenceList> incidence_lists_;
    };

    template <typename Weight>
    DirectedWeightedGraph<Weight>::DirectedWeightedGraph(size_t vertex_count)
        : incidence_lists_(vertex_count)
    {
    }

    template <typename Weight>
    EdgeId DirectedWeightedGraph<Weight>::AddEdge(const Edge<Weight> &edge)
    {
        edges_.push_back(edge);
        const EdgeId id = edges_.size() - 1;
        incidence_lists_.at(edge.from).push_back(id);
        return id;
    }

    template <typename Weight>
    void DirectedWeightedGraph<Weight>::AddVertexCount(size_t count)
    {
        incidence_lists_.resize(count, std::deque<EdgeId>{});
    }

    template <typename Weight>
    size_t DirectedWeightedGraph<Weight>::GetVertexCount() const
    {
        return incidence_lists_.size();
    }

    template <typename Weight>
    size_t DirectedWeightedGraph<Weight>::GetEdgeCount() const
    {
        return edges_.size();
    }

    template <typename Weight>
    const Edge<Weight> &DirectedWeightedGraph<Weight>::GetEdge(EdgeId edge_id) const
    {
        return edges_.at(edge_id);
    }

    template <typename Weight>
    typename DirectedWeightedGraph<Weight>::IncidentEdgesRange
    DirectedWeightedGraph<Weight>::GetIncidentEdges(VertexId vertex) const
    {
        return ranges::AsRange(incidence_lists_.at(vertex));
    }
} // namespace graph