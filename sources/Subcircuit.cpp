#include "../headers/abeai.h"
#include "../headers/debug.h"

std::vector<Node*> Subcircuit::get_nodes() const {
    std::set<Node*> node_set;
    std::function<void(Node*)> dfs = [&](Node* node) {
        node_set.insert(node);
        for (Node* lower_node : node->lower)
            if (!node_set.count(lower_node))
                dfs(lower_node);
    };
    for (Edge* edge : upper_edges)
        dfs(edge->lower);
    std::vector<Node*> nodes;
    std::copy(node_set.begin(), node_set.end(), std::back_inserter(nodes));
    return nodes;
}

Subcircuit& Subcircuit::copy() const {
    std::map<Node*, Node*> old_to_new;
    std::function<void(Node*)> dfs = [&](Node* node) {
        old_to_new[node] = new Node(node->type);
        for (Node* lower_node : node->lower)
            if (!old_to_new.count(lower_node))
                dfs(lower_node);
    };
    for (Edge* edge : upper_edges)
        dfs(edge->lower);
    for (const auto& [old_node, new_node] : old_to_new) {
        for (Node* upper_node : old_node->upper)
            new_node->upper.insert(old_to_new[upper_node]);
        for (Node* lower_node : old_node->lower)
            new_node->lower.insert(old_to_new[lower_node]);
    }
    std::vector<Edge*> new_upper_edges;
    for (Edge* edge : upper_edges)
        new_upper_edges.push_back(new Edge(nullptr, old_to_new[edge->lower]));
    std::vector<Edge*> new_lower_edges;
    for (Edge* edge : lower_edges)
        new_lower_edges.push_back(new Edge(old_to_new[edge->upper], nullptr));
    return *new Subcircuit(new_upper_edges, new_lower_edges);
}
