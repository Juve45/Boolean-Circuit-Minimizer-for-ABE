#include "../headers/abeai.h"
#include "../headers/debug.h"

int main() {
    std::ofstream fout1("inputs/formulas_small.txt");
    int count1 = 0;
    while (true) {
        const Circuit circuit = Build::random(Random::integer(20, 26), Random::integer(2, 6));
        std::string formula = Logic::to_formula(circuit);
        Tree *tree = &Logic::to_tree(formula);
        tree->trim();
        formula = Logic::to_formula(*tree);
        const int size = tree->get_cost();
        if (20 <= size && size <= 40) {
            fout1 << formula << '\n';
            if (++count1 == 50) break;
        }
    }
    std::ofstream fout2("inputs/formulas_big.txt");
    int count2 = 0;
    while (true) {
        const Circuit circuit = Build::random(Random::integer(20, 26), Random::integer(2, 6));
        std::string formula = Logic::to_formula(circuit);
        Tree *tree = &Logic::to_tree(formula);
        tree->trim();
        formula = Logic::to_formula(*tree);
        const int size = tree->get_cost();
        if (size >= 50) {
            fout2 << formula << '\n';
            if (++count2 == 50) break;
        }
    }
    return 0;
}
