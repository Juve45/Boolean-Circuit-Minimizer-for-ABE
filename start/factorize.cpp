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

// TO DO: instead of sending the initial formula send the tree and make a deep copy function
// to copy the three for each iteration
Tree* iterated_hc(std::string formula, int runs = 100) {
    int best_cost = 1e9;
    Tree *best_tree;
    
    for (int i=1;i<=runs;i++){
        Tree *tree = &Logic::to_tree(formula);
        tree->trim();
        hill_climbing(tree);
        int cost = tree->get_cost();
        if (cost < best_cost) {
            best_cost = cost;
            best_tree = tree;
        }
    }

    return best_tree;
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

int improvement_percent(int old_val, int new_val) {
    if (old_val == new_val)
        return 0;
    return ceil((old_val - new_val) * 100 / old_val);
}

int main() {
    std::ifstream fin("inputs/formulas.txt");
    std::string formula;
    int total_formulas = 0, trim_improvement_sum = 0, hc_improvement_sum = 0, hc_trim_improvement_sum = 0;
    int iterated_hc_improvement_sum = 0;
    while (fin >> formula) {
        total_formulas++;
        Tree *tree = &Logic::to_tree(formula);

        std::cout << formula << '\n';
        int before_trim = tree->get_cost();
        std::cout << "Cost before trim: " << before_trim << '\n';
        
        tree->trim();
        std::cout << "Trimmed formula: " << tree->formula << '\n';
        int after_trim = tree->get_cost();
        std::cout << "Cost after trim: " << after_trim << '\n';

        hill_climbing(tree);
        std::cout << tree->formula << "\n";
        int after_hc = tree->get_cost();
        std::cout << "Cost after hc: " << after_hc << "\n\n";

        Tree* iterated_hc_tree = iterated_hc(formula);
        std::cout << iterated_hc_tree->formula << "\n";
        int after_ihc = iterated_hc_tree->get_cost();
        std::cout << "Cost after ihc: " << after_ihc << "\n\n";

        trim_improvement_sum += improvement_percent(before_trim, after_trim);
        hc_improvement_sum += improvement_percent(after_trim, after_hc);
        hc_trim_improvement_sum += improvement_percent(before_trim, after_hc);
        iterated_hc_improvement_sum += improvement_percent(after_trim, after_ihc);
    }

    std::cout << "Trim improvement average: " << 1.0 * trim_improvement_sum / total_formulas << '\n';
    std::cout << "Hc improvement average: " << 1.0 * hc_improvement_sum / total_formulas << '\n';
    std::cout << "Hc + trim improvement average: " << 1.0 * hc_trim_improvement_sum / total_formulas << '\n';
    std::cout << "Ihc improvement average: " << 1.0 * iterated_hc_improvement_sum / total_formulas << '\n';
    
    
    return 0;
}
