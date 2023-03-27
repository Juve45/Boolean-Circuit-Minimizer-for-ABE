#include "../headers/abeai.h"
#include "../headers/debug.h"

std::istream& operator>>(std::istream& in, NodeType& node_type) {
    std::string name; in >> name;
    if (name == "AND") node_type = AND;
    if (name == "OR") node_type = OR;
    if (name == "FO") node_type = FAN_OUT;
    if (name == "INPUT") node_type = INPUT;
    return in;
}

std::ostream& operator<<(std::ostream& out, const NodeType& node_type) {
    if (node_type == AND) out << "AND";
    if (node_type == OR) out << "OR";
    if (node_type == FAN_OUT) out << "FO";
    if (node_type == INPUT) out << "INPUT";
    return out;
}

std::ostream& operator<<(std::ostream& out, const Node& node) {
    out << node.type << '(' << node.id << ')';
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

std::istream& operator>>(std::istream& in, Subcircuit& subcircuit) {
    int node_count; in >> node_count;
    int edge_count; in >> edge_count;
    int upper_count; in >> upper_count;
    int lower_count; in >> lower_count;
    std::vector<NodeType> types(node_count);
    for (NodeType& type : types) in >> type;
    std::vector<std::pair<int, int>> edges(edge_count);
    for (auto& [upper, lower] : edges) in >> upper >> lower;
    std::vector<int> upper_nodes(upper_count);
    for (int& node : upper_nodes) in >> node;
    std::vector<int> lower_nodes(lower_count);
    for (int& node : lower_nodes) in >> node;
    subcircuit = Build::from(types, edges, upper_nodes, lower_nodes);
    return in;
}

std::ostream& operator<<(std::ostream& out, const Tree& tree) {
    std::function<void(Tree, int)> dfs = [&](Tree node, int level) {
        for (int i = 0; i < level; i++)
            std::cout << " | ";
        std::cout << node.type << ' ' << node.formula << '\n';
        for (Tree* child : node.children)
            dfs(*child, level + 1);
    };
    dfs(tree, 0);
    return out;
}
