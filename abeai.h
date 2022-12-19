#pragma once
#include <set>
#include <vector>

enum NodeType {
    AND, OR, FAN_OUT, INPUT
};

struct Node {
    static int node_count;
    static map<int, Node*> fromId;
    int id;
    NodeType type;
    std::set<Node*> top, bottom;
    Node(NodeType type) : type(type) {
        id = nodeCount++;
        fromId[id] = this;
    }

    ~Node() {
        fromId.erase(this->id);
    }
};

struct Edge {
    int id;
    Node *top, *bottom;
};

class SubCircuit {
    std::vector<Edge*> topEdges, bottomEdges;
};

class Circuit {
    Node *root;
    std::vector<Node*> leaves;

public:
    Circuit(Node* root, const std::vector<Node*>& leaves) :
        root(root), leaves(leaves) { }
    void print(); // debugging
    int eval(); // using cost function (number of paths)
    void replaceSubCircuit(const SubCircuit& circuit1, const SubCircuit& circuit2);
};

class CircuitBuilder {
    int height; // length of maximum path from root to some leaf
    int nodeCount; // total number of nodes
    int leafCount; // number of leaves

public:
    CircuitBuilder(int height, int nodeCount, int leafCount) :
        height(height), nodeCount(nodeCount), leafCount(leafCount) { }
    Circuit& build();
};

class PatternFinder {
    std::map<int, int> mapping;
    std::map<int, Node*> nodeMapping;
    SubCircuit& findPattern(Circuit& circuit, const SubCircuit& pattern);
};
