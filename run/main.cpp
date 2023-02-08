#include "../headers/abeai.h"
#include "../headers/debug.h"

int main() {
    Subcircuit pattern = CircuitBuilder::from({AND, OR, OR, FAN_OUT}, {
        {0, 1},
        {0, 2},
        {1, 3},
        {2, 3}
    }, {0}, {1, 3, 2});

    Subcircuit replacement = CircuitBuilder::from({OR, AND}, {
        {0, 1}
    }, {0}, {1, 0, 1});

    std::cout << "==========\n";
    while (true) {
        Circuit circuit = CircuitBuilder::random(50, 2);
        std::vector<int> values(1, circuit.eval());
        while (true) {
            Subcircuit *match = PatternFinder::find_pattern(circuit, pattern);
            if (match == nullptr) break;
            circuit.replace_subcircuit(*match, replacement.copy());
            values.push_back(circuit.eval());
        }
        if (values.size() > 1) {
            for (const int value : values)
                std::cout << value << '\n';
            std::cout << "==========\n";
        }
    }
    return 0;
}
