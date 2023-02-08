#include "../headers/abeai.h"
#include "../headers/debug.h"

int Node::node_count;
// std::map<int, Node*> Node::from_id;

Node::Node(NodeType type) : type(type) {
    id = node_count++;
    // from_id[id] = this;
}

Node::~Node() {
    // from_id.erase(this->id);
}
