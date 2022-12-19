#include <iostream>
#include "abeai.h"

int main() {
    Circuit circuit = CircuitBuilder(4, 7, 3).build();
    circuit.print();
    circuit.copy().print();
    
    std::vector<std::pair<SubCircuit, SubCircuit>> to_replace;

    to_replace.push_back();

    std::cout << "Circuit value: " << circuit.eval() << '\n';

    PatternFinder pf;

    for(int i = 1; i <= 100; i++) { 

        Circuit copy_circuit = circuit.copy(); // creeaza noduri cu id nou
        // alegem random subcircuit
        auto found = pf.findPattern(copy_circuit, to_replace[0].first);
        if(found.topEdges.size() != 0) {
            copy_circuit.replaceSubCircuit(found, to_replace[0].second); // 
        }
    }

    return 0;
}
