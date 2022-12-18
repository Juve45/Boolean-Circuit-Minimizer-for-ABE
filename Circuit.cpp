#include <functional>
#include <iostream>
#include "abeai.h"

// https://csacademy.com/app/graph_editor/
void Circuit::print() {
    std::set<Node*> visited;
    auto stringify = [&](NodeType type) {
        if (type == AND) return "AND";
        if (type == OR) return "OR";
        if (type == FAN_OUT) return "FAN_OUT";
        if (type == INPUT) return "INPUT";
        return "";
    };
    std::function<void(Node*)> dfs = [&](Node* node) {
        visited.insert(node);
        for (Node* bottomNode : node->bottom) {
            std::cout << node->id << '-' << stringify(node->type) << ' ';
            std::cout << bottomNode->id << '-' << stringify(bottomNode->type) << '\n';
            if (!visited.count(bottomNode))
                dfs(bottomNode);
        }
    };
    dfs(root);
}

int main() {
    CircuitBuilder(4, 7, 3).build().print();
    return 0;
}
