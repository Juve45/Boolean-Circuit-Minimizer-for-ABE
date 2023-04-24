#include "../headers/abeai.h"
#include "../headers/debug.h"

int main() {
    // std::ofstream fout1("inputs/formulas_small.txt");
    // int count1 = 0;
    // while (true) {
    //     const Circuit circuit = Build::random(Random::integer(20, 26), Random::integer(2, 6));
    //     std::string formula = Logic::to_formula(circuit);
    //     Tree *tree = &Logic::to_tree(formula);
    //     tree->trim();
    //     formula = Logic::to_formula(*tree);
    //     const int size = tree->get_cost();
    //     if (20 <= size && size <= 40) {
    //         fout1 << formula << '\n';
    //         if (++count1 == 50) break;
    //     }
    // }
    // std::ofstream fout2("inputs/formulas_big.txt");
    // int count2 = 0;
    // while (true) {
    //     const Circuit circuit = Build::random(Random::integer(20, 50), Random::integer(2, 4));
    //     std::string formula = Logic::to_formula(circuit);
    //     Tree *tree = &Logic::to_tree(formula);
    //     tree->trim();
    //     formula = Logic::to_formula(*tree);
    //     const int size = tree->get_cost();
    //     if (size >= 50) {
    //         fout2 << formula << '\n';
    //         if (++count2 == 50) break;
    //     }
    // }
    // std::ofstream fout3("inputs/formulas_small_2.txt");
    // int count3 = 0;
    // while (true) {
    //     const Circuit circuit = Build::random(20, Random::integer(2, 4));
    //     std::string formula = Logic::to_formula(circuit);
    //     Tree *tree = &Logic::to_tree(formula);
    //     tree->trim();
    //     formula = Logic::to_formula(*tree);
    //     const int size = tree->get_cost();
    //     if (60 <= size && size <= 90) {
    //         fout3 << formula << '\n';
    //         if (++count3 == 50) break;
    //     }
    // }
    // std::ofstream fout4("inputs/formulas_big_2.txt");
    // int count4 = 0;
    // while (true) {
    //     const Circuit circuit = Build::random(Random::integer(25, 35), Random::integer(2, 4));
    //     std::string formula = Logic::to_formula(circuit);
    //     Tree *tree = &Logic::to_tree(formula);
    //     tree->trim();
    //     formula = Logic::to_formula(*tree);
    //     const int size = tree->get_cost();
    //     if (160 <= size && size <= 200) {
    //         fout4 << formula << '\n';
    //         if (++count4 == 10) break;
    //     }
    // }
    std::ofstream fout5("inputs/formulas_really_big.txt");
    int count5 = 0;
    while (true) {
        const Circuit circuit = Build::random(Random::integer(50, 101), Random::integer(2, 4));
        std::string formula = Logic::to_formula(circuit);
        Tree *tree = &Logic::to_tree(formula);
        tree->trim();
        formula = Logic::to_formula(*tree);
        const int size = tree->get_cost();
        if (350 <= size && size <= 450) {
            fout5 << formula << '\n';
            std::cout << ++count5 << '\n';
            if (count5 == 50) break;
        }
    }
    return 0;
}
