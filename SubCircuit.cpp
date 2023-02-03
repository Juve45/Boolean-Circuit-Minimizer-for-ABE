#include <functional>
#include "abeai.h"

SubCircuit::SubCircuit(const Circuit& circuit) {
    for (Node* leaf : circuit.leaves)
        this->bottom_edges.push_back(new Edge(leaf, nullptr));
    this->top_edges.push_back(new Edge(nullptr, circuit.root));
}

SubCircuit::SubCircuit(std::vector <Edge*> top_edges, std::vector<Edge*> bottom_edges) {
    for (Edge* edge : bottom_edges)
        this->bottom_edges.push_back(edge);
    for (Edge* edge : top_edges)
        this->top_edges.push_back(edge);
    //TODO: assert that the subcircuit is good (connex?)
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


std::ostream& operator<<(std::ostream& out, const SubCircuit& subcircuit) {
    out << "IN EDGES: ";
    for(auto e : subcircuit.top_edges)
        out << *e << ' ';
    out << '\n';
    out << "OUT EDGES: ";
    for(auto e : subcircuit.bottom_edges)
        out << *e << ' ';
    out << '\n';
    return out;
}