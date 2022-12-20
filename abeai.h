#include <map>
#include <set>
#include <vector>

enum NodeType {
    AND, OR, FAN_OUT, INPUT
};

struct Node;
struct Edge;
struct Circuit;
struct SubCircuit;
class CircuitBuilder;
class PatternFinder;

struct Node {
    static int node_count;
    static std::map<int, Node*> from_id;

    int id;
    NodeType type;
    std::set<Node*> top, bottom;

    Node(NodeType type);
    ~Node();
};

struct Edge {
    Node *top, *bottom;
    Edge(Node* top, Node* bottom) : top(top), bottom(bottom) { }
};

struct Circuit {
    Node *root;
    std::vector<Node*> leaves;

    Circuit(Node* root, const std::vector<Node*>& leaves) :
        root(root), leaves(leaves) { }
    Circuit& copy();

    int eval();
    void print();

    void replace_subcircuit(
        const SubCircuit& found,
        const SubCircuit& to_replace
    );
    static Circuit& from(
        const std::vector<NodeType>& types,
        const std::vector<std::pair<int, int>>& edges
    );
};

struct SubCircuit {
    std::vector<Edge*> top_edges, bottom_edges;
    SubCircuit();
    SubCircuit(const Circuit& circuit);
};

class CircuitBuilder {
    int height;
    int node_count;
    int leaf_count;

public:
    CircuitBuilder(int height, int node_count, int leaf_count) :
        height(height), node_count(node_count), leaf_count(leaf_count) { }
    Circuit& build();
};

class PatternFinder {
    std::map<int, int> mapping;
    SubCircuit create_sub_from_nodes(
        std::vector<int> node_list,
        const SubCircuit& pattern
    );
    bool isomorph(
        const std::vector<int>& list_circuit,
        const std::vector<int>& list_pattern,
        const SubCircuit& pattern
    );

public:
    SubCircuit find_pattern(Circuit& circuit, const SubCircuit& pattern);
};
