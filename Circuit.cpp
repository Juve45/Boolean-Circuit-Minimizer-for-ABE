#include <functional>
#include <iostream>
#include <queue>
#include <map>
#include <algorithm>
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

int Circuit::eval() {
    std::map<int, int> node_top_visited = std::map<int, int>(); 
    std::map<int, int> node_value = std::map<int, int>(); 

    std::queue<std::pair<Node*, int> > nodes;
    nodes.push({this->root, 1});

    int ans = 0;

    while (!nodes.empty()){
        auto &[node, value] = nodes.front();
        nodes.pop();

        for (Node* next_node : node->bottom) {
            node_top_visited[next_node->id]++;
            node_value[next_node->id] += value;
            if (node_top_visited[next_node->id] == next_node->top.size()) {
                nodes.push({next_node, node_value[next_node->id]});
            }
        }

        if (node->type == NodeType::INPUT) {
            ans += value;
        }
    }

    return ans;
}

int main() {
    Circuit circuit = CircuitBuilder(4, 7, 3).build();
    circuit.print();
    
    vector <pair<SubCircuit, SubCricuit>> to_replace;

    std::cout << "Circuit value: " << circuit.eval() << '\n';

    for(int i = 1; i <= 100; i++) { 

        Circuit copy_circuit = circuit.copy(); // creeaza noduri cu id nou
        // alegem random subcircuit
        auto found = PatternFinder.findPattern(copy_circuit, to_replace[0].first);
        if(found.topEdges.size() != 0) {
            copy_circuit.replace(found); // 
        }
    }

    return 0;
}
