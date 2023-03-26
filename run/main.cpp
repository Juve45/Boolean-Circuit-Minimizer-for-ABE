#include "../headers/abeai.h"
#include "../headers/debug.h"
#include "../headers/patterns.h"
#include "../headers/Tree.h"

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout << std::boolalpha;
    load_patterns();

	ifstream fin("formulas.txt");
	string formula;
	while (fin >> formula) {
		cout << formula << '\n';
        Circuit circuit = Utils::to_circuit(formula);
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
        cout << Utils::to_formula(circuit) << "\n\n";
	}
	return 0;
}
