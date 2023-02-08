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
    static std::map<int, Node*> from_id;

    int id;
    NodeType type;
    std::set<Node*> upper, lower;

    Node(NodeType type) : type(type) { from_id[id = node_count++] = this; }
    ~Node() { from_id.erase(this->id); }
};

struct Edge {
    Node *upper, *lower;
    Edge(Node* upper, Node* lower) : upper(upper), lower(lower) { }
};

struct Circuit {
    Node *root;
    std::vector<Node*> leaves;

    Circuit(Node* root, const std::vector<Node*>& leaves) : root(root), leaves(leaves) { }
    std::vector<Node*> get_nodes() const;
    Circuit& copy() const;
    int eval() const;
    void replace_subcircuit(const Subcircuit& found, const Subcircuit& replacement);
};

struct Subcircuit {
    std::vector<Edge*> upper_edges, lower_edges;
    Subcircuit(const std::vector<Edge*>& upper_edges, const std::vector<Edge*>& lower_edges) : upper_edges(upper_edges), lower_edges(lower_edges) { }
    std::vector<Node*> get_nodes() const;
    Subcircuit& copy() const;
};

struct CircuitBuilder {
    static Circuit& random(int leaf_count, int max_lower_count);
    static Circuit& random(int height, int node_count, int leaf_count);
    static Circuit& from(const std::vector<NodeType>& types, const std::vector<std::pair<int, int>>& edges);
    static Subcircuit& from(const std::vector<NodeType>& types, const std::vector<std::pair<int, int>>& edges, const std::vector<int>& upper_nodes, const std::vector<int>& lower_nodes);
};

struct PatternFinder {
    static Subcircuit* find_pattern(const Circuit& circuit, const Subcircuit& pattern);
};

struct Utils {
    static void check_circuit(const Circuit& circuit);
    static std::vector<int> random_partition(int value, int parts);
};

std::ostream& operator<<(std::ostream& out, const Node& node);
std::ostream& operator<<(std::ostream& out, const Edge& edge);
std::ostream& operator<<(std::ostream& out, const Circuit& circuit);
std::ostream& operator<<(std::ostream& out, const Subcircuit& subcircuit);

#endif
