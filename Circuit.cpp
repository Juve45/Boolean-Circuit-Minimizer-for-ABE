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
    std::map<int, int> nodeTopCisited = std::map<int, int>(); 
    std::map<int, int> nodeValue = std::map<int, int>(); 

    std::queue<std::pair<Node*, int> > nodes;
    nodes.push({this->root, 1});

    int ans = 0;

    while (!nodes.empty()){
        auto &[node, value] = nodes.front();
        nodes.pop();

        for (Node* nextNode : node->bottom) {
            nodeTopCisited[nextNode->id]++;
            nodeValue[nextNode->id] += value;
            if (nodeTopCisited[nextNode->id] == nextNode->top.size()) {
                nodes.push({nextNode, nodeValue[nextNode->id]});
            }
        }

        if (node->type == NodeType::INPUT) {
            ans += value;
        }
    }

    return ans;
}

void replaceSubCircuit(const SubCircuit& subCircuit) {
    for (Edge* edge : subCircuit.topEdges) {

    }
}

int main() {
    Circuit circuit = CircuitBuilder(4, 7, 3).build();
    circuit.print();
    
    std::vector<std::pair<SubCircuit, SubCircuit>> to_replace;

    std::cout << "Circuit value: " << circuit.eval() << '\n';

    for(int i = 1; i <= 100; i++) { 

        Circuit copy_circuit = circuit.copy(); // creeaza noduri cu id nou
        // alegem random subcircuit
        auto found = PatternFinder.findPattern(copy_circuit, to_replace[0].first);
        if(found.topEdges.size() != 0) {
            copy_circuit.replace(found, to_replace[0].second); // 
        }
    }

    return 0;
}
