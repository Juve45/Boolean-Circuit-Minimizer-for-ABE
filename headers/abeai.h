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
struct Build;
struct Tree;
struct Factorizer;
struct Logic;
struct Random;

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
    void check() const;
    Subcircuit* find_pattern(const Subcircuit& pattern) const;
    void replace_subcircuit(const Subcircuit& found, const Subcircuit& replacement);
};

struct Subcircuit {
    std::vector<Edge*> upper_edges, lower_edges;
    Subcircuit() { }
    Subcircuit(const std::vector<Edge*>& upper_edges, const std::vector<Edge*>& lower_edges) : upper_edges(upper_edges), lower_edges(lower_edges) { }
    std::vector<Node*> get_nodes() const;
    Subcircuit& copy() const;
};

struct Build {
    static Circuit& random(int leaf_count, int max_lower_count);
    static Circuit& random(int height, int node_count, int leaf_count);
    static Circuit& from(const std::vector<NodeType>& types, const std::vector<std::pair<int, int>>& edges);
    static Subcircuit& from(const std::vector<NodeType>& types, const std::vector<std::pair<int, int>>& edges, const std::vector<int>& upper_nodes, const std::vector<int>& lower_nodes);
};

struct Tree {
    NodeType type;
    std::string formula;
    Tree *parent;
    std::vector<Tree*> children;
    Tree(NodeType type) : type(type) { }
    void update_formula();
    void add_child(Tree* child);
    void erase_child(Tree* child);
    void trim();
    int get_cost() const;
    bool has_child(const std::string& formula) const;
};

struct Factorizer {
    static std::vector<std::vector<Tree*>> reduce(Tree* t);
    static void factorize(Tree* t1, Tree* t2);
    static void defactorize(Tree* t1, Tree* t2);
};

struct Logic {
    static Circuit& to_circuit(const std::string& formula);
    static std::string to_formula(const Circuit& circuit);
    static Tree& to_tree(const std::string& formula);
    static std::string to_formula(const Tree& tree);
    static Subcircuit& flipped(const Subcircuit& pattern1);
};

struct Random {
    static int integer(int max);
    static int integer(int min, int max);
    static std::pair<int, int> two_integers(int max);
    static std::vector<int> partition(int value, int parts);
};

std::istream& operator>>(std::istream& in, NodeType& node_type);
std::ostream& operator<<(std::ostream& out, const NodeType& node_type);
std::ostream& operator<<(std::ostream& out, const Node& node);
std::ostream& operator<<(std::ostream& out, const Edge& edge);
std::ostream& operator<<(std::ostream& out, const Circuit& circuit);
std::ostream& operator<<(std::ostream& out, const Subcircuit& subcircuit);
std::istream& operator>>(std::istream& in, Subcircuit& subcircuit);
std::ostream& operator<<(std::ostream& out, const Tree& tree);

#endif
