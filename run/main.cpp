#include "../headers/abeai.h"
#include "../headers/debug.h"
#include "../headers/patterns.h"

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout << std::boolalpha;

    load_patterns();
    for (int try_id = 0; ; try_id++) {
        Circuit circuit = CircuitBuilder::random(13, 2);
        std::vector<int> values(1, circuit.eval());
        int not_found_count = 0, index = 0;
        while (not_found_count < int(patterns.size()) / 2) {
            Subcircuit *match = PatternFinder::find_pattern(circuit, patterns[index]);
            if (match == nullptr)
                not_found_count++;
            else {
                not_found_count = 0;
                circuit.replace_subcircuit(*match, replacements[index].copy());
                values.push_back(circuit.eval());
            }
            index = (index + 1) % patterns.size();
        }
        if (values.size() > 1) {
            std::cout << "========== " << try_id << '\n';
            for (const int value : values)
                std::cout << value << '\n';
        }
    }
    return 0;
}
