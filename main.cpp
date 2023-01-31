#include <iostream>
#include "abeai.h"
#include "debug.h"

int main() {

    Circuit circuit = CircuitBuilder::random(5, 2);

    std::cout << circuit << '\n';

    Circuit circuit1 = CircuitBuilder::from({AND, OR, OR, FAN_OUT, INPUT, INPUT, INPUT}, {
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
    SubCircuit pattern = *new SubCircuit(CircuitBuilder::from({AND, OR, OR, INPUT, FAN_OUT, INPUT, INPUT}, {
        {0, 1},
        {0, 2},
        {1, 3},
        {1, 4},
        {2, 4},
        {2, 5},
        {4, 6}
    }));
    SubCircuit *match = PatternFinder::find_pattern(circuit, pattern);
    if (match != nullptr) {
        for (Edge* edge : match->top_edges)
            std::cout << *edge << '\n';
        std::cout << '\n';
        for (Edge* edge : match->bottom_edges)
            std::cout << *edge << '\n';
        std::cout << '\n';
    }
    return 0;

    // // Circuit circuit = CircuitBuilder::random(5, 45, 5);
    // Circuit circuit = CircuitBuilder::random(5, 2);
    // std::cout << circuit << '\n';

    // Circuit circuit1 = CircuitBuilder::from({AND, OR, OR, FAN_OUT, INPUT, INPUT, INPUT}, {
    //     {0, 1},
    //     {0, 2},
    //     {1, 4},
    //     {1, 3},
    //     {2, 3},
    //     {2, 6},
    //     {3, 5}
    // });
    // Circuit circuit2 = CircuitBuilder::from({OR, AND, INPUT, INPUT, INPUT}, {
    //     {0, 1},
    //     {0, 3},
    //     {1, 2},
    //     {1, 4}
    // });
    // // std::cout << circuit1 << '\n';
    // // std::cout << circuit2 << '\n';

    // std::vector<std::pair<SubCircuit, SubCircuit>> to_replace;
    // to_replace.emplace_back(circuit1, circuit2);

    // std::cout << "Circuit value: " << circuit.eval() << '\n';

    // PatternFinder pf;

    // for(int i = 1; i <= 30; i++) {

    //     std::cout << ".";

    //     Circuit copy_circuit = circuit.copy(); // creeaza noduri cu id nou
    //     // alegem random subcircuit
    //     auto found = pf.find_pattern(copy_circuit, to_replace[0].first);
    //     if (found != nullptr) {
    //         copy_circuit.replace_subcircuit(*found, to_replace[0].second); //
    //         dbg_ok;
    //         std::cout << "Circuit value: " << copy_circuit.eval() << '\n';
    //     }
    // }
    // return 0;
}
