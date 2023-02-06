#include <iostream>
#include "abeai.h"
#include "debug.h"
// #include "patterns.h"

int main() {
    
    Circuit circuit = CircuitBuilder::random(5, 2);
    Circuit circuit2 = CircuitBuilder::from({OR, AND, INPUT, OR, OR, OR, FAN_OUT, INPUT, INPUT, INPUT, INPUT}, {
        {0, 1},
        {0, 2},
        {1, 3},
        {1, 4},
        {3, 5},
        {3, 6},
        {4, 6},
        {4, 7},
        {5, 8},
        {5, 9},
        {6, 10}
    });
    Circuit circuit3 = CircuitBuilder::from({INPUT, INPUT, INPUT, INPUT, INPUT, OR, AND, AND, FAN_OUT, OR, OR, FAN_OUT, AND}, {
        {12, 9},
        {12, 10},
        {9, 6},
        {9, 11},
        {10, 11},
        {10, 5},
        {6, 1},
        {6, 2},
        {11, 7},
        {5, 0},
        {5, 8},
        {7, 8},
        {7, 3},
        {8, 4}
    });
    std::cout << circuit3 << '\n';

    SubCircuit pattern, to_replace;
    Node *node0 = new Node(AND);
    Node *node1 = new Node(OR);
    Node *node2 = new Node(OR);
    Node *node4 = new Node(FAN_OUT);
    node0->bottom.insert(node1); node1->top.insert(node0);
    node0->bottom.insert(node2); node2->top.insert(node0);
    node1->bottom.insert(node4); node4->top.insert(node1);
    node2->bottom.insert(node4); node4->top.insert(node2);
    pattern.top_edges.push_back(new Edge(nullptr, node0));
    pattern.bottom_edges.push_back(new Edge(node1, nullptr)); // A
    pattern.bottom_edges.push_back(new Edge(node2, nullptr)); // C
    pattern.bottom_edges.push_back(new Edge(node4, nullptr)); // B


    Node *node5 = new Node(OR);
    Node *node6 = new Node(AND);
    // node5->top.insert(nullptr);
    node5->bottom.insert(node6); node6->top.insert(node5);
    // node5->bottom.insert(nullptr); // B
    to_replace.bottom_edges.push_back(new Edge(node6, nullptr));
    to_replace.bottom_edges.push_back(new Edge(node6, nullptr));
    to_replace.bottom_edges.push_back(new Edge(node5, nullptr));
    to_replace.top_edges.push_back(new Edge(nullptr, node5));

    SubCircuit *match = PatternFinder::find_pattern(circuit3, pattern);

    dbg(match);

    dbg(circuit3.eval());


    if (match != nullptr) {
        dbg(*match);
        
        for (Edge* edge : match->top_edges)
            std::cout << *edge << '\n';
        std::cout << '\n';
        for (Edge* edge : match->bottom_edges)
            std::cout << *edge << '\n';
        std::cout << '\n';
    
        circuit3.replace_subcircuit(*match, to_replace);
        dbg(circuit3.eval());
    
        std::cout << circuit3 << '\n';
    }






    return 0;
}

// int main() {

//     Circuit circuit = CircuitBuilder::random(5, 2);

//     std::cout << circuit << '\n';

//     /*Circuit circuit1 = CircuitBuilder::from({AND, OR, OR, FAN_OUT, INPUT, INPUT, INPUT}, {
//         {0, 1},
//         {0, 2},
//         {1, 3},
//         {1, 4},
//         {3, 5},
//         {3, 6},
//         {4, 6},
//         {4, 7},
//         {5, 8},
//         {5, 9},
//         {6, 10}
//     });
//     SubCircuit pattern = *new SubCircuit(CircuitBuilder::from({AND, OR, OR, INPUT, FAN_OUT, INPUT, INPUT}, {
//         {0, 1},
//         {0, 2},
//         {1, 3},
//         {1, 4},
//         {2, 4},
//         {2, 5},
//         {4, 6}
//     }));
//     SubCircuit *match = PatternFinder::find_pattern(circuit, pattern);
//     if (match != nullptr) {
//         for (Edge* edge : match->top_edges)
//             std::cout << *edge << '\n';
//         std::cout << '\n';
//         for (Edge* edge : match->bottom_edges)
//             std::cout << *edge << '\n';
//         std::cout << '\n';
//     }*/

//     for (Edge* edge : pattern1.top_edges)
//         std::cout << *edge << '\n';
//     std::cout << '\n';
//     for (Edge* edge : to_replace1.bottom_edges)
//         std::cout << *edge << '\n';

//     return 0;

//     // // Circuit circuit = CircuitBuilder::random(5, 45, 5);
//     // Circuit circuit = CircuitBuilder::random(5, 2);
//     // std::cout << circuit << '\n';

//     // Circuit circuit1 = CircuitBuilder::from({AND, OR, OR, FAN_OUT, INPUT, INPUT, INPUT}, {
//     //     {0, 1},
//     //     {0, 2},
//     //     {1, 4},
//     //     {1, 3},
//     //     {2, 3},
//     //     {2, 6},
//     //     {3, 5}
//     // });
//     // Circuit circuit2 = CircuitBuilder::from({OR, AND, INPUT, INPUT, INPUT}, {
//     //     {0, 1},
//     //     {0, 3},
//     //     {1, 2},
//     //     {1, 4}
//     // });
//     // // std::cout << circuit1 << '\n';
//     // // std::cout << circuit2 << '\n';

//     // std::vector<std::pair<SubCircuit, SubCircuit>> to_replace;
//     // to_replace.emplace_back(circuit1, circuit2);

//     // std::cout << "Circuit value: " << circuit.eval() << '\n';

//     // PatternFinder pf;

//     // for(int i = 1; i <= 30; i++) {

//     //     std::cout << ".";

//     //     Circuit copy_circuit = circuit.copy(); // creeaza noduri cu id nou
//     //     // alegem random subcircuit
//     //     auto found = pf.find_pattern(copy_circuit, to_replace[0].first);
//     //     if (found != nullptr) {
//     //         copy_circuit.replace_subcircuit(*found, to_replace[0].second); //
//     //         dbg_ok;
//     //         std::cout << "Circuit value: " << copy_circuit.eval() << '\n';
//     //     }
//     // }
//     // return 0;
// }
