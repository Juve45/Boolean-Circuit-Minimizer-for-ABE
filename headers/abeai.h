#ifndef _ABEAI_H_
#define _ABEAI_H_

#include <bits/stdc++.h>

enum NodeType {
    AND, OR, FAN_OUT, INPUT
};

struct Node;
struct Edge;
struct Circuit;
struct Subcircuit;
struct CircuitBuilder;
struct PatternFinder;
struct Utils;

struct Node {
    static int node_count;
    // static std::map<int, Node*> from_id;

    int id;
    NodeType type;
    std::set<Node*> upper, lower;

    Node(NodeType type);
    ~Node();
};

struct Edge {
    Node *upper, *lower;
    Edge(Node* upper, Node* lower) : upper(upper), lower(lower) { }
};

struct Circuit {
    Node *root;
    std::vector<Node*> leaves;

    Circuit(Node* root, const std::vector<Node*>& leaves) : root(root), leaves(leaves) { }
    Circuit& copy();
    std::vector<Node*> get_nodes();
    int eval();
    void replace_subcircuit(const Subcircuit& found, const Subcircuit& to_replace);
};

struct Subcircuit {
    std::vector<Edge*> upper_edges, lower_edges;
    Subcircuit() { }
    Subcircuit(const Circuit& circuit);
    Subcircuit(std::vector<Edge*> upper_edges, std::vector<Edge*> lower_edges);
    Subcircuit(std::vector<NodeType> nodes, std::vector<std::pair<int, int>> edges);
    std::vector<Node*> get_nodes();
};

struct CircuitBuilder {
    static Circuit& random(int leaf_count, int max_lower_count);
    static Circuit& random(int height, int node_count, int leaf_count);
    static Circuit& from(const std::vector<NodeType>& types, const std::vector<std::pair<int, int>>& edges);
};

struct PatternFinder {
    static Subcircuit* find_pattern(Circuit& circuit, Subcircuit& pattern);
};

struct Utils {
    static void check_circuit(Node* root, int leaf_count);
    static std::vector<int> random_partition(int value, int parts);
};

std::ostream& operator<<(std::ostream& out, const Node& node);
std::ostream& operator<<(std::ostream& out, const Edge& edge);
std::ostream& operator<<(std::ostream& out, const Circuit& circuit);
std::ostream& operator<<(std::ostream& out, const Subcircuit& subcircuit);

#endif
