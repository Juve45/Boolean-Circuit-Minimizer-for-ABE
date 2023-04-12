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

    // dbg("A");
    // dbg(best_cost);
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
    for (int k = 0; k < k_max; k++) {
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
}

// ==========================================================================================
// ==========================================================================================
// ==========================================================================================

long double improvement_percent(int old_val, int new_val) {
    return old_val == new_val ? 0 : ceil((old_val - new_val) * 100.0 / old_val);
}

uint64_t current_time_ms() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

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

void replace(Circuit& circuit) {
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
}

void test_first_formula() {
    std::ifstream fin("inputs/formulas_small.txt");
    std::string formula;
    
    fin >> formula;
    
    Tree *tree = &Logic::to_tree(formula);
    std::cout << formula << "\n";
    std::cout << tree->get_cost() << "\n\n";

    tree->trim();

    std::cout << "After trim: " << tree->formula << "\n\n";

    hill_climbing(tree);

    std::cout << "After hc: " << tree->formula << '\n';
}

int main() {
    load_patterns();
    const int ITERATION_COUNT = 5;

    std::vector<long double> time(3);
    std::vector<long double> score(3);

    int formula_count;
    for (int i = 0; i < ITERATION_COUNT; i++) {
        std::ifstream fin("inputs/formulas_real.txt");
        std::cout << "started iteration #" << i << '\n';

        formula_count = 0;
        std::string formula;
        while (fin >> formula) {
            if (i == 0) {
                Circuit circuit = Logic::to_circuit(formula);
                long double t01 = current_time_ms();
                long double s01 = circuit.eval();
                std::string f01 = Logic::to_formula(circuit);
                replace(circuit);
                long double t02 = current_time_ms();
                long double s02 = circuit.eval();
                std::string f02 = Logic::to_formula(circuit);
                time[0] = t02 - t01;
                score[0] = improvement_percent(s01, s02);
            }

            Tree *tree1 = &Logic::to_tree(formula);
            long double t11 = current_time_ms();
            long double s11 = tree1->get_cost();
            // std::string f11 = Logic::to_formula(*tree1);
            hill_climbing(tree1);
            long double t12 = current_time_ms();
            long double s12 = tree1->get_cost();
            // std::string f12 = Logic::to_formula(*tree1);

            Tree *tree2 = &Logic::to_tree(formula);
            long double t21 = current_time_ms();
            long double s21 = tree2->get_cost();
            // std::string f21 = Logic::to_formula(*tree2);
            tree2 = iterated_hc(formula);
            long double t22 = current_time_ms();
            long double s22 = tree2->get_cost();
            // std::string f22 = Logic::to_formula(*tree2);

            std::cout << "finished formula #" << formula_count << '\n';
            formula_count++;
            // time[0] += t02 - t01; score[0] += improvement_percent(s01, s02);
            time[1] += t12 - t11; score[1] += improvement_percent(s11, s12);
            time[2] += t22 - t21; score[2] += improvement_percent(s21, s22);
            // putem afișa pe aici f01/f02/f11/f12/f21/f22 pentru debugging
        }
    }

    time[0] /= 1000;
    // time[0] /= ITERATION_COUNT * formula_count * 1000;
    time[1] /= ITERATION_COUNT * formula_count * 1000;
    time[2] /= ITERATION_COUNT * formula_count * 1000;

    // score[0] /= ITERATION_COUNT * formula_count;
    score[1] /= ITERATION_COUNT * formula_count;
    score[2] /= ITERATION_COUNT * formula_count;

    std::cout << "replace time: " << time[0] << '\n';
    std::cout << "     hc time: " << time[1] << '\n';
    std::cout << "    ihc time: " << time[2] << '\n';
    std::cout << '\n';

    std::cout << "replace score: " << score[0] << '\n';
    std::cout << "     hc score: " << score[1] << '\n';
    std::cout << "    ihc score: " << score[2] << '\n';
    return 0;
}
