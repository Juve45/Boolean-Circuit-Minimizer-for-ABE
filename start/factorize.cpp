#include "../headers/abeai.h"
#include "../headers/debug.h"
#include <chrono>

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

long double improvement_percent(int old_val, int new_val) {
    if (old_val == new_val)
        return 0;
    return ceil((old_val - new_val) * 100.0 / old_val);
}

uint64_t timeSinceEpochMillisec() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
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

int test_replace(const std::string& formula) {
    Circuit circuit = Logic::to_circuit(formula);
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
    return circuit.eval();
}

std::vector<long double> run_algorithms(bool debug = false) {
    std::ifstream fin("inputs/formulas_big.txt");
    std::string formula;

    int total_formulas = 0;
    long double replace_improvement_sum = 0, trim_improvement_sum = 0, hc_improvement_sum = 0, hc_trim_improvement_sum = 0;
    long double iterated_hc_improvement_sum = 0;
    // int sa_improvement_sum = 0;

    uint64_t ihc_duration_sum = 0, hc_duration_sum = 0, replace_duration_sum = 0;
    while (fin >> formula) {
        total_formulas++;
        Tree *tree = &Logic::to_tree(formula);

        if (debug) std::cout << formula << '\n';
        int before_trim = tree->get_cost();
        if (debug) std::cout << "Cost before trim: " << before_trim << '\n';

        tree->trim();
        if (debug) std::cout << "Trimmed formula: " << tree->formula << '\n';
        int after_trim = tree->get_cost();
        if (debug) std::cout << "Cost after trim: " << after_trim << '\n';

        uint64_t before_replace = timeSinceEpochMillisec();
        int after_replace_cost = test_replace(Logic::to_formula(*tree));
        uint64_t after_replace = timeSinceEpochMillisec();

        uint64_t before_hc = timeSinceEpochMillisec();
        hill_climbing(tree);
        uint64_t after_hc = timeSinceEpochMillisec();
        if (debug) std::cout << tree->formula << "\n";
        int after_hc_cost = tree->get_cost();
        // dbg("Z");
        // dbg(after_hc_cost);
        if (debug) std::cout << "Cost after hc: " << after_hc << "\n";


        uint64_t before_ihc = timeSinceEpochMillisec();
        Tree* iterated_hc_tree = iterated_hc(formula);
        uint64_t after_ihc = timeSinceEpochMillisec();
        if (debug) std::cout << iterated_hc_tree->formula << "\n";
        int after_ihc_cost = iterated_hc_tree->get_cost();
        // dbg("B");
        // dbg(after_ihc_cost);
        if (debug) std::cout << "Cost after ihc: " << after_ihc_cost << "\n\n";

        // Tree *tree2 = &Logic::to_tree(formula);
        // tree2->trim();
        // simulated_annealing(tree2);
        // int after_sa = tree2->get_cost();
        // std::cout << "Cost after sa: " << after_sa << "\n\n";

        trim_improvement_sum += improvement_percent(before_trim, after_trim);
        replace_improvement_sum += improvement_percent(before_trim, after_replace_cost);
        hc_improvement_sum += improvement_percent(after_trim, after_hc_cost);
        hc_trim_improvement_sum += improvement_percent(before_trim, after_hc_cost);
        iterated_hc_improvement_sum += improvement_percent(before_trim, after_ihc_cost);
        // sa_improvement_sum += improvement_percent(after_trim, after_sa);

        replace_duration_sum += after_replace - before_replace;
        hc_duration_sum += after_hc - before_hc;
        ihc_duration_sum += after_ihc - before_ihc;
    }
    fin.close();

    // std::cout << "Hc improvement average: " << 1.0 * hc_improvement_sum / total_formulas << '\n';


    long double trim_average = 1.0 * trim_improvement_sum / total_formulas;
    long double replace_average = 1.0 * replace_improvement_sum / total_formulas;
    long double hc_trim_average = 1.0 * hc_trim_improvement_sum / total_formulas;
    long double ihc_trim_average = 1.0 * iterated_hc_improvement_sum / total_formulas;
    long double hc_duration_average_s = (long double)hc_duration_sum / total_formulas / 1000;
    long double replace_duration_average_s = (long double)replace_duration_sum / total_formulas / 1000;
    long double ihc_duration_average_s = (long double)ihc_duration_sum / total_formulas / 1000;

    // dbg("END");
    // dbg(hc_trim_average);
    // dbg(ihc_trim_average);

    return std::vector<long double>({trim_average, hc_trim_average, ihc_trim_average, hc_duration_average_s, ihc_duration_average_s, replace_average, replace_duration_average_s});
}

void get_formula_size() {
    std::ifstream fin("inputs/formulas.txt");
    std::string formula;

    while (fin >> formula) {
        Tree *tree = &Logic::to_tree(formula);
        std::cout << formula << "\n";
        std::cout << tree->get_cost() << "\n\n";
    }
}

int main() {

    load_patterns();
    int iterations = 30;
    long double trim_average_sum = 0;
    long double hc_trim_average_sum = 0;
    long double ihc_trim_average_sum = 0;
    long double replace_average_sum = 0;
    long double hc_duration_average_sum = 0;
    long double ihc_duration_average_sum = 0;
    long double replace_duration_average_sum = 0;

    for (int i=1;i<=iterations;i++) {
        std::cout << "Iteration #" << i << '\n';

        std::vector<long double> ans = run_algorithms();
        long double trim_average = ans[0];
        long double hc_trim_average = ans[1];
        long double ihc_trim_average = ans[2];
        long double hc_duration_average_s = ans[3];
        long double ihc_duration_average_s = ans[4];
        long double replace_average = ans[5];
        long double replace_duration_average_s = ans[6];

        trim_average_sum += trim_average;
        hc_trim_average_sum += hc_trim_average;
        ihc_trim_average_sum += ihc_trim_average;
        replace_average_sum += replace_average;
        hc_duration_average_sum += hc_duration_average_s;
        ihc_duration_average_sum += ihc_duration_average_s;
        replace_duration_average_sum += replace_duration_average_s;
    }

    std::cout << "Trim improvement average: " << trim_average_sum / iterations << '\n';
    std::cout << "Hc + trim improvement average: " << hc_trim_average_sum / iterations << '\n';
    std::cout << "Ihc + trim improvement average: " << ihc_trim_average_sum / iterations << '\n';
    std::cout << "Replace + trim improvement average: " << replace_average_sum / iterations << '\n';
    // std::cout << "Sa improvement average: " << 1.0 * sa_improvement_sum / total_formulas << '\n';

    std::cout << "\n\n" << "Hc duration: " << std::fixed << std::setprecision(3) << hc_duration_average_sum / iterations << "s\n";
    std::cout << "Ihc duration: " << ihc_duration_average_sum / iterations << "s\n";
    std::cout << "Replace duration: " << replace_duration_average_sum / iterations << "s\n";

    return 0;
}
