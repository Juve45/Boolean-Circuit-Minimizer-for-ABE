#include <functional>
#include "abeai.h"

SubCircuit::SubCircuit(const Circuit& circuit) {
    for (Node* leaf : circuit.leaves)
        this->bottom_edges.push_back(new Edge(leaf, nullptr));
    this->top_edges.push_back(new Edge(nullptr, circuit.root));
}

std::vector<Node*> SubCircuit::get_nodes() {
    std::set<Node*> node_set;
    std::function<void(Node*)> dfs = [&](Node* node) {
        node_set.insert(node);
        for (Node* bottom_node : node->bottom)
            if (!node_set.count(bottom_node))
                dfs(bottom_node);
    };
    for (Edge* edge : top_edges)
        dfs(edge->bottom);
    for (Edge* edge : bottom_edges)
        dfs(edge->top);
    std::vector<Node*> nodes;
    std::copy(node_set.begin(), node_set.end(), std::back_inserter(nodes));
    return nodes;
}
