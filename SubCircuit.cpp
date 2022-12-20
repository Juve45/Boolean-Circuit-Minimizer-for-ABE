#include "abeai.h"

SubCircuit::SubCircuit() {

}

SubCircuit::SubCircuit(const Circuit& circuit) {

    // dbg(circuit.leaves);
    // dbg(circuit.root);

    for(auto leaf : circuit.leaves)
        this->bottom_edges.push_back(new Edge(leaf, NULL));
    this->top_edges.push_back(new Edge(NULL, circuit.root));
}
