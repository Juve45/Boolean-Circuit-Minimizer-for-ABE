#include "../headers/abeai.h"
#include "../headers/debug.h"

void hill_climbing(Tree* t) {
    while (true) {
        std::vector<std::vector<Tree*>> factorizable = our_reduce(t);
        if (factorizable.empty()) break; // we can't optimize further
        int c = Random::integer(factorizable.size());
        assert(factorizable[c].size() > 1);
        auto [f1, f2] = Random::two_integers(factorizable[c].size());
        factorize(factorizable[c][f1], factorizable[c][f2]);
        dbg(t->formula);
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
        for (auto i : node->children)
            nodes.push(i);
    }
    int i = Random::integer(and_nodes.size());
    return and_nodes[i];
}

void simulated_annealing(Tree* root, int k_max = 100) {
    for (int k = 0; k < k_max; k++)
        if (Random::integer(2 * k_max) < k_max - k) {
            // defactorize
            Tree *and_node = get_random_and(root);
            std::vector<Tree*> or_nodes;
            for (auto i : and_node->children)
                if (i->type == OR)
                    or_nodes.push_back(i);
            if (or_nodes.size() >= 2) {
                auto [f1, f2] = Random::two_integers(or_nodes.size());
                defactorize(or_nodes[f1], or_nodes[f2]);
            }
        }
        else {
            // factorize
            std::vector<std::vector<Tree*>> factorizable = our_reduce(root);
            if (factorizable.empty()) continue; // we can't optimize further
            int c = Random::integer(factorizable.size());
            assert(factorizable[c].size() > 1);
            auto [f1, f2] = Random::two_integers(factorizable[c].size());
            factorize(factorizable[c][f1], factorizable[c][f2]);
        }
}

int main() {
    std::ifstream fin("inputs/formulas.txt");
    std::string formula;
    while (fin >> formula) {
        std::cout << formula << '\n';
        Tree *tree = &Tree::from(formula);
        hill_climbing(tree);
        std::cout << tree->formula << "\n\n";
    }
    return 0;
}
