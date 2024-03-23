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
        return IteratorWrapper<IteratorType>(graph[v].begin(), graph[v].end());
    }

    inline int num_nodes() const { return graph.size(); }
    inline int degree(int v) const { return graph[v].size(); }

    std::vector<std::vector<EdgeType>> graph;
};

template <typename T>
AdjacencyList<T> remap_graph(AdjacencyList<T> &graph, std::vector<int> &permutation)
{
    std::vector<T> remapped_graph(graph.size());

    // map position in array
    for (uint v = 0; v < graph.size(); v++)
    {
        remapped_graph[permutation[v]] = graph[v];
    }

    // map edge ids
    for (uint v = 0; v < graph.size(); v++)
    {
        for (uint i = 0; i < graph[v].size(); i++)
        {
            remapped_graph[v][i].id = permutation[remapped_graph[v][i].id];
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
            q.push(e.id);
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
        id = compute_dfs_order_rec(graph, mapping, id, e.id);
    }
    return id;
}

template <typename EdgeType>
struct AdjacencyArray
{
    using IteratorType = std::vector<EdgeType>::iterator;

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