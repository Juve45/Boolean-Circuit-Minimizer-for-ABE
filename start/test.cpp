#include "../headers/abeai.h"
#include "../headers/debug.h"


Tree* hill_climbing(Tree* t) {
    while (true) {
        t->trim();
        std::vector<std::vector<Tree*>> factorizable = Factorizer::reduce(t);
        if (factorizable.empty()) break; // we can't optimize further
        const int c = Random::integer(factorizable.size());
        assert(factorizable[c].size() > 1);
        const auto [f1, f2] = Random::two_integers(factorizable[c].size());
        Factorizer::factorize(factorizable[c][f1], factorizable[c][f2]);
    }
    return t;
}


Tree* real_hill_climbing(Tree * t, int d = 0) {
    if(d == 6) {
        hill_climbing(t);
        return t;
    }
    Tree * cand = t;
    for(int i = 0; i < 5; i++) {
        Tree * t2 = t->deep_copy();

        std::vector<std::vector<Tree*>> factorizable = Factorizer::reduce(t2);
        
        if (factorizable.empty()) 
            break; // we can't optimize further
        
        const int c = Random::integer(factorizable.size());
        
        assert(factorizable[c].size() > 1);
        
        const auto [f1, f2] = Random::two_integers(factorizable[c].size());

        Factorizer::factorize(factorizable[c][f1], factorizable[c][f2]);

        if(t2->get_cost() < cand->get_cost())
            cand = t2;
        // else t2->deep_erase();
    }
    return real_hill_climbing(cand, d + 1);
}


// TO DO: instead of sending the initial formula send the tree and make a deep copy function
// to copy the three for each iteration
Tree* iterated_hc(Tree * original) {
    int runs = 100;
    int best_cost = 1e9;
    Tree *best_tree;

    for (int i=1;i<=runs;i++){
        Tree *tree = original->deep_copy();
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

bool has_or(std::vector<Tree*> nodes) {
    for (Tree* i : nodes) {
        if (i->type == OR)
            return true;
    }
    return false;
}

// Returns a random and node which has at least two children
// and at least one OR child.
// If there is no such AND node, it will return null
Tree* get_random_and(Tree* root) {
    std::vector<Tree*> and_nodes;
    std::queue<Tree*> nodes;
    nodes.push(root);
    while (!nodes.empty()) {
        Tree *node = nodes.front();
        if (node->type == AND && node->children.size() >= 2 && has_or(node->children))
            and_nodes.push_back(node);
        nodes.pop();
        for (Tree* i : node->children)
            nodes.push(i);
    }

    if (and_nodes.size() == 0)
        return nullptr;

    const int i = Random::integer(and_nodes.size());
    return and_nodes[i];
}

void defactorize(Tree* root) {
    Tree *and_node = get_random_and(root);
    // if and_node is null we can't defactorize
    if (!and_node) {
        return;
    }

    std::vector<Tree*> or_nodes;
    for (Tree* i : and_node->children)
        if (i->type == OR)
            or_nodes.push_back(i);
    
    assert(or_nodes.size() >= 1);
    assert(and_node->children.size() >= 2);

    int f1 = Random::integer(or_nodes.size());
    int f2 = Random::integer(and_node->children.size());
    while (or_nodes[f1] == and_node->children[f2]) {
        f2 = Random::integer(and_node->children.size());
    }
    Factorizer::defactorize(or_nodes[f1], and_node->children[f2]);
}

// Return true if we could factorize
bool factorize(Tree* root) {
    std::vector<std::vector<Tree*>> factorizable = Factorizer::reduce(root);
    if (factorizable.empty()) return false; // we can't optimize further
    const int c = Random::integer(factorizable.size());
    assert(factorizable[c].size() > 1);
    const auto [f1, f2] = Random::two_integers(factorizable[c].size());
    Factorizer::factorize(factorizable[c][f1], factorizable[c][f2]);
    return true;
}

Tree* simulated_annealing(Tree* root) {
    int k_max = 200;
    for (int k = 0; k < k_max; k++) {
        root->trim();
        if (Random::integer(20 * k_max) < k_max - k && k > 40) { // defactorize
            defactorize(root);
        }
        else { // factorize
            if (!factorize(root)) {
                continue;
            }
        }
    }
    root->trim();
    while(factorize(root))
        root->trim();
    return root;
}

Tree* iterated_simulated_annealing(Tree * original) {    
    int mn = 1e9;
    Tree * tmn;
    for (int i=1;i<=20;i++) {
        Tree *tree = original->deep_copy();
        tree->trim();
        simulated_annealing(tree);
        int tt = tree->get_cost();
        if(mn > tt) {
            tmn = tree;
            mn = tt;
        }
    }
    return tmn;
}

// ==========================================================================================
// ==========================================================================================
// ==========================================================================================

long double improvement_percent(int old_val, int new_val) {
    return std::max((long double)0, old_val == new_val ? (long double)0 : ceil((old_val - new_val) * (long double)100 / old_val));
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
    std::ifstream fin("inputs/formulas_real.txt");
    std::string formula;

    fin >> formula;

    Tree *tree = &Logic::to_tree(formula);
    std::cout << formula << "\n";
    // dbg(*tree);
    // std::cout << tree->get_cost() << "\n\n";

    tree->trim();
    // dbg(*tree);

    // defactorize(tree);

    // dbg(*tree);

    // factorize(tree);

    // dbg(*tree);

    // return;

    std::cout << "After trim: " << tree->formula << "\n\n";

    simulated_annealing(tree);

    std::cout << "After sa: " << tree->formula << '\n';
}

std::mutex st_lock;

void run_algorithm(std::string formula, Tree * (*algorithm)(Tree *), long double& ttime, long double& score) {
    Tree *tree1 = &Logic::to_tree(formula);
    long double t_start = current_time_ms();
    long double s_start = tree1->get_cost();
    
    tree1 = algorithm(tree1);
    long double t_end = current_time_ms();
    long double s_end = tree1->get_cost();
    

    ttime += t_start - t_end;
    score += improvement_percent(s_start, s_end);

    tree1->clean();
    delete tree1;
}


void iteration(std::string formula, std::vector <long double> &time, std::vector <long double> &score) {

    std::vector<long double> itime(6);
    std::vector<long double> iscore(6);

    std::vector <Tree*(*)(Tree *)> alg;
    alg.push_back(&hill_climbing);
    alg.push_back(&iterated_hc);

    for(int i = 0; i < alg.size(); i++) 
        run_algorithm(formula, alg[i], itime[i], iscore[i]);

    st_lock.lock();

    for(int i = 0; i < alg.size(); i++) {
        time[i] += itime[i];
        score[i] += iscore[i];
    }

    st_lock.unlock();

}



int main(int argc, char* argv[]) {

    // for (int i=1;i<=1000;i++)
    //     test_first_formula();
    // return 0;

    load_patterns();
    const int ITERATION_COUNT = 30;

    std::vector<long double> time(6);
    std::vector<long double> score(6);
    std::vector<std::thread> threads;

    int formula_count;
    for (int i = 0; i < ITERATION_COUNT; i++) {
        std::ifstream fin(argv[1]);
        std::cout << "started iteration #" << i << '\n';

        formula_count = 0;
        std::string formula;
        while (fin >> formula) {
            dbg(formula);
            /*if (i == 0 && false) {
                Circuit circuit = Logic::to_circuit(formula);
                long double t01 = current_time_ms();
                long double s01 = circuit.eval();
                // std::string f01 = Logic::to_formula(circuit);
                replace(circuit);
                long double t02 = current_time_ms();
                long double s02 = circuit.eval();
                // std::string f02 = Logic::to_formula(circuit);
                time[0] = t02 - t01;
                score[0] = improvement_percent(s01, s02);
            }*/

            std::thread t(iteration, formula, std::ref(time), std::ref(score));
            threads.push_back(std::move(t));
            // iteration(formula, time, score);

            std::cout << "finished formula #" << formula_count << '\n';
            formula_count++;

            // putem afișa pe aici f01/f02/f11/f12/f21/f22/f31/f32 pentru debugging
        }
    }

    for(auto& thread : threads)
        thread.join();

    time[0] /= ITERATION_COUNT * formula_count * 1000;
    time[1] /= ITERATION_COUNT * formula_count * 1000;
    time[2] /= ITERATION_COUNT * formula_count * 1000;
    time[3] /= ITERATION_COUNT * formula_count * 1000;
    time[5] /= ITERATION_COUNT * formula_count * 1000;

    score[0] /= ITERATION_COUNT * formula_count;
    score[1] /= ITERATION_COUNT * formula_count;
    score[2] /= ITERATION_COUNT * formula_count;
    score[3] /= ITERATION_COUNT * formula_count;
    // score[4] /= ITERATION_COUNT * formula_count;
    score[5] /= ITERATION_COUNT * formula_count;

    // std::cout << "replace time: " << time[0] << '\n';
    std::cout << "     hc time: " << time[0] << '\n';
    std::cout << "    ihc time: " << time[1] << '\n';
    std::cout << "     sa time: " << time[2] << '\n';
    // std::cout << "    rhc time: " << time[4] << '\n';
    std::cout << "    isa time: " << time[3] << '\n';
    std::cout << '\n';

    // std::cout << "replace score: " << score[0] << '\n';
    std::cout << "     hc score: " << score[0] << '\n';
    std::cout << "    ihc score: " << score[1] << '\n';
    std::cout << "     sa score: " << score[2] << '\n';
    // std::cout << "    rhc score: " << score[4] << '\n';
    std::cout << "    isa score: " << score[3] << '\n';
    return 0;
}
