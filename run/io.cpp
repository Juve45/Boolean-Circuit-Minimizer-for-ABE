#include "../headers/abeai.h"
#include "../headers/debug.h"

std::ostream& operator<<(std::ostream& out, const Node& node) {
    if (node.type == AND) out << "AND";
    if (node.type == OR) out << "OR";
    if (node.type == FAN_OUT) out << "FO";
    if (node.type == INPUT) out << "INPUT";
    out << '(' << node.id << ')';
    return out;
}

std::ostream& operator<<(std::ostream& out, const Edge& edge) {
    const auto& [node1, node2] = edge;
    out << '[';
    if (node1 == nullptr) out << "null"; else out << *node1;
    out << ' ';
    if (node2 == nullptr) out << "null"; else out << *node2;
    out << ']';
    return out;
}

std::ostream& operator<<(std::ostream& out, const Circuit& circuit) {
    std::set<Node*> visited;
    std::function<void(Node*)> dfs = [&](Node* node) {
        visited.insert(node);
        for (Node* lower_node : node->lower) {
            out << *node << ' ' << *lower_node << '\n';
            if (!visited.count(lower_node))
                dfs(lower_node);
        }
    };
    dfs(circuit.root);
    out << "root: " << *circuit.root << '\n';
    out << "leaves: ";
    for (Node* leaf : circuit.leaves)
        out << *leaf << ' ';
    out << '\n';
    return out;
}

std::ostream& operator<<(std::ostream& out, const Subcircuit& subcircuit) {
    std::set<Node*> visited;
    std::function<void(Node*)> dfs = [&](Node* node) {
        visited.insert(node);
        for (Node* lower_node : node->lower) {
            out << *node << ' ' << *lower_node << '\n';
            if (!visited.count(lower_node))
                dfs(lower_node);
        }
    };
    for (Edge* edge : subcircuit.upper_edges)
        dfs(edge->lower);
    for (int i = 0; i < int(subcircuit.upper_edges.size()); i++)
        out << "UPPER(" << i << ") " << *subcircuit.upper_edges[i]->lower << '\n';
    for (int i = 0; i < int(subcircuit.lower_edges.size()); i++)
        out << *subcircuit.lower_edges[i]->upper << " LOWER(" << i << ")\n";
    out << "upper edges: ";
    for (Edge* edge : subcircuit.upper_edges)
        out << *edge << ' ';
    out << '\n';
    out << "lower edges: ";
    for (Edge* edge : subcircuit.lower_edges)
        out << *edge << ' ';
    out << '\n';
    return out;
}