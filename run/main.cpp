#include "../headers/abeai.h"
#include "../headers/debug.h"

int main() {
    Subcircuit pattern;
    Node *node0 = new Node(AND);
    Node *node1 = new Node(OR);
    Node *node2 = new Node(OR);
    Node *node4 = new Node(FAN_OUT);
    node0->lower.insert(node1); node1->upper.insert(node0);
    node0->lower.insert(node2); node2->upper.insert(node0);
    node1->lower.insert(node4); node4->upper.insert(node1);
    node2->lower.insert(node4); node4->upper.insert(node2);
    pattern.upper_edges.push_back(new Edge(nullptr, node0));
    pattern.lower_edges.push_back(new Edge(node1, nullptr));
    pattern.lower_edges.push_back(new Edge(node2, nullptr));
    pattern.lower_edges.push_back(new Edge(node4, nullptr));

    Subcircuit replacement;
    Node *node5 = new Node(OR);
    Node *node6 = new Node(AND);
    node5->lower.insert(node6); node6->upper.insert(node5);
    replacement.lower_edges.push_back(new Edge(node6, nullptr));
    replacement.lower_edges.push_back(new Edge(node6, nullptr));
    replacement.lower_edges.push_back(new Edge(node5, nullptr));
    replacement.upper_edges.push_back(new Edge(nullptr, node5));

    std::cout << "==========\n";
    while (true) {
        Circuit circuit = CircuitBuilder::random(50, 2);
        std::cout << circuit.eval() << '\n';
        while (true) {
            Subcircuit *match = PatternFinder::find_pattern(circuit, pattern);
            if (match == nullptr) break;
            circuit.replace_subcircuit(*match, replacement);
            std::cout << circuit.eval() << '\n';
            std::cout << "==========\n";
        }
    }
    return 0;
}
