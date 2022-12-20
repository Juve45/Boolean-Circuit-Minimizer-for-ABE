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
