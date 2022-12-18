#include <set>
#include <vector>

enum NodeType {
    AND, OR, FAN_OUT, INPUT
};

struct Node {
    static int nodeCount;
    int id;
    NodeType type;
    std::set<Node*> top, bottom;
    Node(NodeType type) : type(type) {
        // id = nodeCount++;
    }
};

struct Edge {
    static int edgeCount;
    int id;
    Node *top, *bottom;
    Edge() {
        id = edgeCount++;
    }
};

class SubCircuit {
    std::vector<Edge*> topEdges, bottomEdges;
};

class Circuit {
    Node *root;
    std::vector<Node*> leaves;

public:
    Circuit(Node* root, std::vector<Node*>& leaves) :
        root(root), leaves(leaves) { }

    int eval(); // using cost function (number of paths)
    void swapSubCircuit(
		const SubCircuit& current,
        const SubCircuit& newCircuit
	);
};

class CircuitBuilder {
    int height; // length of maximum path from root to some leaf
    int nodeCount; // number of internal nodes
    int leafCount; // number of leaves

public:
    CircuitBuilder(int height, int nodeCount, int leafCount);
    Circuit& build();
};

class PatternFinder {
    SubCircuit& findPattern(Circuit& circuit, const SubCircuit& patternCircuit);
};
