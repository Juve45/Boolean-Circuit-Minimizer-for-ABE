#include "../headers/abeai.h"
#include "../headers/debug.h"

Subcircuit::Subcircuit(const Circuit& circuit) {
    for (Node* leaf : circuit.leaves)
        this->lower_edges.push_back(new Edge(leaf, nullptr));
    this->upper_edges.push_back(new Edge(nullptr, circuit.root));
}

Subcircuit::Subcircuit(const std::vector<Edge*>& upper_edges, const std::vector<Edge*>& lower_edges) {
    for (Edge* edge : lower_edges)
        this->lower_edges.push_back(edge);
    for (Edge* edge : upper_edges)
        this->upper_edges.push_back(edge);
    // TODO: assert that the subcircuit is good (connex?)
}

Subcircuit::Subcircuit(const std::vector<NodeType>& nodes, const std::vector<std::pair<int, int>>& edges) {
    dbg_ok;
    std::vector<Node*> ptr_nodes;
    for (const NodeType type : nodes) {
        ptr_nodes.push_back(new Node(type));
        dbg_ok;
    }
    for (auto edge : edges) {
        dbg(edge);
        Node *from = nullptr;
        Node *to = nullptr;

        if (edge.first != -1)
            from = ptr_nodes[edge.first];
        if (edge.second != -1)
            to = ptr_nodes[edge.second];

        from->lower.insert(to);
        to->upper.insert(from);

        if (from == nullptr)
            this->upper_edges.push_back(new Edge(from, to));
        if (to == nullptr)
            this->lower_edges.push_back(new Edge(from, to));
    }
}

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
    for (Edge* edge : lower_edges)
        dfs(edge->upper);
    std::vector<Node*> nodes;
    std::copy(node_set.begin(), node_set.end(), std::back_inserter(nodes));
    return nodes;
}