#pragma once

#include <vector>
#include <queue>

template <typename Iter>
struct IteratorWrapper
{
    IteratorWrapper(Iter first, Iter last) : first(first), last(last) {}

    Iter begin() const { return first; }
    Iter end() const { return last; }

    Iter first;
    Iter last;
};

template <typename EdgeType>
struct AdjacencyList
{
    using IteratorType = std::vector<EdgeType>::iterator;

    IteratorWrapper<IteratorType> neighbors(int v)
    {
        return IteratorWrapper<IteratorType>(edges[v].begin(), edges[v].end());
    }

    inline int num_nodes() const { return edges.size(); }
    inline int degree(int v) const { return edges[v].size(); }

    std::vector<std::vector<EdgeType>> edges;
};

template <typename EdgeType>
AdjacencyList<EdgeType> remap_graph(AdjacencyList<EdgeType> &graph, std::vector<int> &permutation)
{
    std::vector<std::vector<EdgeType>> remapped_graph(graph.num_nodes());

    // map position in array
    for (int v = 0; v < graph.num_nodes(); v++)
    {
        remapped_graph[permutation[v]] = graph.edges[v];
    }

    // map edge ids
    for (int v = 0; v < graph.num_nodes(); v++)
    {
        for (uint i = 0; i < remapped_graph[v].size(); i++)
        {
            int new_id = permutation[remapped_graph[v][i].get_id()];
            remapped_graph[v][i].set_id(new_id);
        }
    }
    return AdjacencyList{remapped_graph};
}

template <typename Graph>
std::vector<int> compute_bfs_order(Graph &graph, int start_node)
{
    int id = 0;
    std::vector<int> bfs_order(graph.num_nodes());
    std::queue<int> q;
    q.push(start_node);
    while (!q.empty())
    {
        int v = q.front();
        q.pop();
        bfs_order[v] = id++;
        for (auto &e : graph.neighbors(v))
        {
            q.push(e.get_id());
        }
    }
    return bfs_order;
}

template <typename Graph>
std::vector<int> compute_dfs_order(Graph &graph, int start_node)
{
    std::vector<int> dfs_order(graph.num_nodes());
    compute_dfs_order_rec(graph, dfs_order, 0, start_node);
    return dfs_order;
}

template <typename Graph>
int compute_dfs_order_rec(Graph &graph, std::vector<int> &mapping, int id, int v)
{
    mapping[v] = id++;
    for (auto &e : graph.neighbors(v))
    {
        id = compute_dfs_order_rec(graph, mapping, id, e.get_id());
    }
    return id;
}

template <typename EdgeType>
struct AdjacencyArray
{
    using IteratorType = std::vector<EdgeType>::iterator;

    AdjacencyArray() {}

    AdjacencyArray(AdjacencyList<EdgeType> &graph)
    {
        int n = graph.num_nodes();
        nodes.resize(n + 1);

        // compute indices as prefix sum of degrees
        for (int v = 0; v < n; v++)
        {
            nodes[v + 1] += graph.degree(v) + nodes[v];
        }

        // insert edges
        edges.reserve(nodes.back());
        for (int v = 0; v < n; v++)
        {
            for (auto e : graph.neighbors(v))
            {
                edges.push_back(e);
            }
        }
    }

    static AdjacencyArray construct_with_bfs_order(AdjacencyList<EdgeType> &graph)
    {
        auto order = compute_bfs_order(graph, 0);
        auto rearranged = remap_graph(graph, order);
        return AdjacencyArray(rearranged);
    }

    static AdjacencyArray construct_with_dfs_order(AdjacencyList<EdgeType> &graph)
    {
        auto order = compute_dfs_order(graph, 0);
        auto rearranged = remap_graph(graph, order);
        return AdjacencyArray(rearranged);
    }

    inline int num_nodes() const { return nodes.size() - 1; }
    inline int num_edges() const { return edges.size(); }

    IteratorWrapper<IteratorType> neighbors(int v)
    {
        int start = nodes[v];
        int end = nodes[v + 1];
        return IteratorWrapper<IteratorType>(edges.begin() + start, edges.begin() + end);
    }

    std::vector<int> nodes;
    std::vector<EdgeType> edges;
};