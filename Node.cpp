#include <fstream>
#include <map>
#include "abeai.h"

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
