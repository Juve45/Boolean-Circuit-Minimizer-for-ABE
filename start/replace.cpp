#include "../headers/abeai.h"
#include "../headers/debug.h"

std::vector<Subcircuit> patterns;
std::vector<Subcircuit> replacements;

void load_patterns() {
    std::ifstream fin("inputs/patterns.txt");
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(fin, line))
        lines.push_back(line);
    std::string content;
    std::vector<bool> should_include;
    for (int i = 0; i < int(lines.size()); i++)
        if (!lines[i].empty() && lines[i].front() == '-') {
            should_include.push_back(lines[i].back() == '-');
            i += 2;
        }
        else
            content += lines[i] + '\n';
    std::stringstream sin(content);
    for (int i = 0; i < int(should_include.size()); i++) {
        Subcircuit pattern; sin >> pattern;
        Subcircuit replacement; sin >> replacement;
        if (should_include[i]) {
            patterns.push_back(pattern);
            replacements.push_back(replacement);
            patterns.push_back(Logic::flipped(pattern));
            replacements.push_back(Logic::flipped(replacement));
        }
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout << std::boolalpha;

    load_patterns();
    std::ifstream fin("inputs/formulas.txt");
    std::string formula;
    while (fin >> formula) {
        std::cout << formula << '\n';
        Circuit circuit = Logic::to_circuit(formula);
        const int init_cost = circuit.eval();
        std::vector<int> values(1, circuit.eval());
        int not_found_count = 0, index = 0;
        while (not_found_count < 2 * int(patterns.size())) {
            Subcircuit *match = circuit.find_pattern(patterns[index]);
            if (match == nullptr)
                not_found_count++;
            else {
                not_found_count = 0;
                circuit.replace_subcircuit(*match, replacements[index].copy());
                values.push_back(circuit.eval());
            }
            index = (index + 1) % patterns.size();
        }
        std::cout << Logic::to_formula(circuit) << '\n';
        const int final_cost = circuit.eval();
        std::cout << init_cost << " -> " << final_cost << "\n\n";
    }
    return 0;
}
