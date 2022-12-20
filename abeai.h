#ifndef ABEAI_H
#define ABEAI_H
#include <set>
#include <vector>
#include <map>

enum NodeType {
    AND, OR, FAN_OUT, INPUT
};

struct Node {
    static int nodeCount;
    static std::map<int, Node*> fromId;
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

struct SubCircuit;

struct Edge {
    Node *top, *bottom;

    Edge(Node *_top, Node *_bottom) {
        top = _top;
        bottom = _bottom;
    }
};


class Circuit {
public:
    Node *root;
    std::vector<Node*> leaves;

    Circuit(Node* root, const std::vector<Node*>& leaves) :
        root(root), leaves(leaves) { }
    void print(); // debugging
    Circuit& copy();
    int eval(); // using cost function (number of paths)
    void replaceSubCircuit(const SubCircuit& found, const SubCircuit& toReplace);
    static Circuit& from(const std::vector<NodeType>& types, const std::vector<std::pair<int, int>>& edges);
};

struct SubCircuit {
    std::vector<Edge*> topEdges, bottomEdges;

    SubCircuit();
    SubCircuit(const Circuit& circuit);
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
public:
    SubCircuit findPattern(Circuit& circuit, const SubCircuit& pattern);
private:
    std::map<int, int> mapping;
    SubCircuit createSubFromNodes(std::vector<int> node_list, 
        const SubCircuit& pattern);
    bool isomorph(const std::vector<int> &list_circuit, 
    const std::vector <int> &list_pattern, const SubCircuit& pattern);
};


#endif