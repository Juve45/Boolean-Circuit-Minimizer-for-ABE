#include "headers/abeai.h"
#include "headers/debug.h"

int main() {
    SubCircuit pattern;
    Node *node0 = new Node(AND);
    Node *node1 = new Node(OR);
    Node *node2 = new Node(OR);
    Node *node4 = new Node(FAN_OUT);
    node0->bottom.insert(node1); node1->top.insert(node0);
    node0->bottom.insert(node2); node2->top.insert(node0);
    node1->bottom.insert(node4); node4->top.insert(node1);
    node2->bottom.insert(node4); node4->top.insert(node2);
    pattern.top_edges.push_back(new Edge(nullptr, node0));
    pattern.bottom_edges.push_back(new Edge(node1, nullptr));
    pattern.bottom_edges.push_back(new Edge(node2, nullptr));
    pattern.bottom_edges.push_back(new Edge(node4, nullptr));

    SubCircuit replacement;
    Node *node5 = new Node(OR);
    Node *node6 = new Node(AND);
    node5->bottom.insert(node6); node6->top.insert(node5);
    replacement.bottom_edges.push_back(new Edge(node6, nullptr));
    replacement.bottom_edges.push_back(new Edge(node6, nullptr));
    replacement.bottom_edges.push_back(new Edge(node5, nullptr));
    replacement.top_edges.push_back(new Edge(nullptr, node5));

    std::cout << "==========\n";
    while (true) {
        Circuit circuit = CircuitBuilder::random(50, 2);
        std::cout << circuit.eval() << '\n';
        while (true) {
            SubCircuit *match = PatternFinder::find_pattern(circuit, pattern);
            if (match == nullptr) break;
            circuit.replace_subcircuit(*match, replacement);
            std::cout << circuit.eval() << '\n';
            std::cout << "==========\n";
        }
    }
    return 0;
}
