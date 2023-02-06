#include <fstream>
#include <iostream>
#include <map>
#include "abeai.h"
#include "debug.h"

int Node::node_count = 0;
std::map<int, Node*> Node::from_id = std::map<int, Node*>();

Node::Node(NodeType type) : type(type) {
    id = node_count++;
    from_id[id] = this;
}

Node::~Node() {
    from_id.erase(this->id);
}

std::ostream& operator<<(std::ostream& out, const Node& node) {
    if (node.type == AND) out << "AND";
    if (node.type == OR) out << "OR";
    if (node.type == FAN_OUT) out << "FAN_OUT";
    if (node.type == INPUT) out << "INPUT";
    out << '(' << node.id << ')';
    return out;
}

std::ostream& operator<<(std::ostream& out, const Edge& edge) {
    const auto& [node1, node2] = edge;
    if (node1 == nullptr) out << "null"; else out << *node1;
    out << " -> ";
    if (node2 == nullptr) out << "null"; else out << *node2;
    return out;
}
