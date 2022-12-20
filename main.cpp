#include <iostream>
#include "abeai.h"

int main() {
    Circuit circuit = CircuitBuilder::random(4, 7, 3);
    std::cout << circuit << '\n';
    std::cout << circuit.copy() << '\n';

    Circuit circuit1 = CircuitBuilder::from({AND, OR, OR, FAN_OUT, INPUT, INPUT, INPUT}, {
        {0, 1},
        {0, 2},
        {1, 4},
        {1, 3},
        {2, 3},
        {2, 6},
        {3, 5}
    });
    Circuit circuit2 = CircuitBuilder::from({OR, AND, INPUT, INPUT, INPUT}, {
        {0, 1},
        {0, 3},
        {1, 2},
        {1, 4}
    });
    std::cout << circuit1 << '\n';
    std::cout << circuit2 << '\n';

    // std::vector<std::pair<SubCircuit, SubCircuit>> to_replace;
    // to_replace.emplace_back(
    //     Circuit::from({AND, OR, OR, FAN_OUT, INPUT, INPUT, INPUT}, {
    //         {0, 1},
    //         {0, 2},
    //         {1, 4},
    //         {1, 3},
    //         {2, 3},
    //         {2, 6},
    //         {3, 5}
    //     }),
    //     Circuit::from({OR, AND, INPUT, INPUT, INPUT}, {
    //         {0, 1},
    //         {0, 3},
    //         {1, 2},
    //         {1, 4}
    //     })
    // );

    // std::cout << "Circuit value: " << circuit.eval() << '\n';

    // PatternFinder pf;

    // for(int i = 1; i <= 100; i++) {

    //     Circuit copy_circuit = circuit.copy(); // creeaza noduri cu id nou
    //     // alegem random subcircuit
    //     auto found = pf.findPattern(copy_circuit, to_replace[0].first);
    //     if(found.topEdges.size() != 0) {
    //         copy_circuit.replaceSubCircuit(found, to_replace[0].second); //
    //     }
    // }

    return 0;
}
