#include "../headers/abeai.h"
#include "../headers/debug.h"
#include "../headers/patterns.h"

int main() {
    std::string formula1 = "((((a+b+c)*(b+c))*d)+(a+b+c)+b+c)";
    Circuit circuit = Utils::to_circuit(formula1);
    std::string formula2 = Utils::to_formula(circuit);
    std::cout << formula1 << "\n\n";
    std::cout << circuit << '\n';
    std::cout << formula2 << '\n';

    // std::string formula1 = "((((a+b)*(b+c))*d)+(a+b))";
    // Circuit circuit = Utils::to_circuit(formula1);
    // std::string formula2 = Utils::to_formula(circuit);
    // std::cout << formula1 << "\n\n";
    // std::cout << circuit << '\n';
    // std::cout << formula2 << '\n';

    // std::vector<Subcircuit> patterns = {pattern1, pattern2};
    // std::vector<Subcircuit> replacements = {replacement1, replacement2};

    // std::mt19937 rand(std::chrono::steady_clock::now().time_since_epoch().count());
    // for (int try_id = 0; ; try_id++) {
    //     Circuit circuit = CircuitBuilder::random(50, 5);
    //     std::vector<int> values(1, circuit.eval());
    //     int not_found_count = 0;
    //     while (not_found_count < 2 * int(patterns.size())) {
    //         const int index = rand() % patterns.size();
    //         Subcircuit *match = PatternFinder::find_pattern(circuit, patterns[index]);
    //         if (match == nullptr)
    //             not_found_count++;
    //         else {
    //             not_found_count = 0;
    //             circuit.replace_subcircuit(*match, replacements[index].copy());
    //             values.push_back(circuit.eval());
    //         }
    //     }
    //     if (values.size() > 1) {
    //         std::cout << "========== " << try_id << '\n';
    //         for (const int value : values)
    //             std::cout << value << '\n';
    //     }
    // }
    return 0;
}
