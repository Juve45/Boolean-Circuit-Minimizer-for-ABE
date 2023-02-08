#include "../headers/abeai.h"
#include "../headers/debug.h"

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

SubCircuit::SubCircuit(std::vector<NodeType> nodes, std::vector <std::pair<int, int>> edges) {

    dbg_ok;
    std::vector <Node*> pNodes;
    for(const NodeType type : nodes) {
        pNodes.push_back(new Node(type));
        dbg_ok;
    }

    for(auto edge : edges) {
        dbg(edge);
        Node * from = nullptr;
        Node * to = nullptr;

        if(edge.first != -1)
            from = pNodes[edge.first];
        if(edge.second != -1)
            to = pNodes[edge.second];

        from->bottom.insert(to);
        to->top.insert(from);

        if(from == nullptr)
            this->top_edges.push_back(new Edge(from, to));
        if(to == nullptr)
            this->bottom_edges.push_back(new Edge(from, to));
    }

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