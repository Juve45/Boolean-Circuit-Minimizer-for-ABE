#include "../headers/abeai.h"
#include "../headers/debug.h"

void hill_climbing(Tree* t) {
    while (true) {
        std::vector<std::vector<Tree*>> factorizable = Factorizer::reduce(t);
        if (factorizable.empty()) break; // we can't optimize further
        const int c = Random::integer(factorizable.size());
        assert(factorizable[c].size() > 1);
        const auto [f1, f2] = Random::two_integers(factorizable[c].size());
        Factorizer::factorize(factorizable[c][f1], factorizable[c][f2]);
    }
}

Tree* get_random_and(Tree* root) {
    std::vector<Tree*> and_nodes;
    std::queue<Tree*> nodes;
    nodes.push(root);
    while (!nodes.empty()) {
        Tree *node = nodes.front();
        if (node->type == AND)
            and_nodes.push_back(node);
        nodes.pop();
        for (Tree* i : node->children)
            nodes.push(i);
    }
    const int i = Random::integer(and_nodes.size());
    return and_nodes[i];
}

void simulated_annealing(Tree* root, int k_max = 100) {
    for (int k = 0; k < k_max; k++)
        if (Random::integer(2 * k_max) < k_max - k) { // defactorize
            Tree *and_node = get_random_and(root);
            std::vector<Tree*> or_nodes;
            for (Tree* i : and_node->children)
                if (i->type == OR)
                    or_nodes.push_back(i);
            if (or_nodes.size() >= 2) {
                const auto [f1, f2] = Random::two_integers(or_nodes.size());
                Factorizer::defactorize(or_nodes[f1], or_nodes[f2]);
            }
        }
        else { // factorize
            std::vector<std::vector<Tree*>> factorizable = Factorizer::reduce(root);
            if (factorizable.empty()) continue; // we can't optimize further
            const int c = Random::integer(factorizable.size());
            assert(factorizable[c].size() > 1);
            const auto [f1, f2] = Random::two_integers(factorizable[c].size());
            Factorizer::factorize(factorizable[c][f1], factorizable[c][f2]);
        }
}

int main() {
    std::ifstream fin("inputs/formulas.txt");
    std::string formula;
    while (fin >> formula) {
        std::cout << formula << '\n';
        Tree *tree = &Logic::to_tree(formula);
        tree->trim();
        std::cout << "Trimmed formula: " << tree->formula << '\n';
        hill_climbing(tree);
        std::cout << tree->formula << "\n\n";
    }
    return 0;
}
