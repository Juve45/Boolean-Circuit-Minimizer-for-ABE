#include "../headers/abeai.h"
#include "../headers/debug.h"

int main() {
    std::ofstream fout("inputs/formulas.txt");
    for (int i = 0; i < 50; i++) {
        const Circuit circuit = Build::random(Random::integer(5, 21), Random::integer(2, 6));
        const std::string formula = Logic::to_formula(circuit);
        fout << formula << '\n';
    }
    return 0;
}
