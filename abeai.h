#include <map>
#include <set>
#include <vector>
#include <ostream>
#include <functional>

enum NodeType {
    AND, OR, FAN_OUT, INPUT
};

struct Node;
struct Edge;
struct Circuit;
struct SubCircuit;
struct CircuitBuilder;
struct PatternFinder;

struct Node {
    static int node_count;
    static std::map<int, Node*> from_id;

    int id;
    NodeType type;
    std::set<Node*> top, bottom;

    Node(NodeType type);
    ~Node();
    friend std::ostream& operator<<(std::ostream& out, const Node& node);
};

struct Edge {
    Node *top, *bottom;
    Edge(Node* top, Node* bottom) : top(top), bottom(bottom) { }
    friend std::ostream& operator<<(std::ostream& out, const Edge& edge);
};

struct Circuit {
    Node *root;
    std::vector<Node*> leaves;

    Circuit(Node* root, const std::vector<Node*>& leaves) : root(root), leaves(leaves) { }
    Circuit& copy();
    std::vector<Node*> get_nodes();
    int eval();
    void replace_subcircuit(const SubCircuit& found, const SubCircuit& to_replace);
    friend std::ostream& operator<<(std::ostream& out, const Circuit& circuit);
};

struct SubCircuit {
    std::vector<Edge*> top_edges, bottom_edges;
    SubCircuit() { }
    SubCircuit(const Circuit& circuit);
    std::vector<Node*> get_nodes();
};

struct CircuitBuilder {
    static Circuit& random(int leaf_count, int max_bottom_count);
    static Circuit& random(int height, int node_count, int leaf_count);
    static Circuit& from(const std::vector<NodeType>& types, const std::vector<std::pair<int, int>>& edges);
};

struct PatternFinder {
    static SubCircuit* find_pattern(Circuit& circuit, SubCircuit& pattern);
};
