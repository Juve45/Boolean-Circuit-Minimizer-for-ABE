#include "../headers/abeai.h"
#include "../headers/debug.h"
#include "../headers/patterns.h"
#include "../headers/Tree.h"

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout << std::boolalpha;

    Tree tree = Tree::from("((a*b*c)+(a*d)+(b*c*d))");
    std::cout << tree << '\n';
    return 0;

    load_patterns();
    int max_delta = 0;
    for (int try_id = 0; ; try_id++) {
        Circuit circuit = CircuitBuilder::random(20, 2);
        std::vector<int> values(1, circuit.eval());
        int not_found_count = 0, index = 0;
        while (not_found_count < 2 * int(patterns.size())) {
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
            const int delta = values.front() - values.back();
            max_delta = std::max(max_delta, delta);
            std::cout << ">>> try #" << try_id << ": ";
            for (const int value : values)
                std::cout << value << ' ';
            std::cout << "\ndelta: " << delta << " (best: " << max_delta << ")\n";
        }
    }
    return 0;
}
