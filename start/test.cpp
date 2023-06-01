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

int ihc_iterations = 60;

// TO DO: instead of sending the initial formula send the tree and make a deep copy function
// to copy the three for each iteration
Tree* iterated_hc(Tree * original) {
    int runs = ihc_iterations;
    int best_cost = 1e9;
    Tree *best_tree = nullptr;

    for (int i=1;i<=runs;i++){
        Tree *tree = original->deep_copy();
        tree->trim();
        hill_climbing(tree);
        int cost = tree->get_cost();
        if (cost < best_cost) {
            best_cost = cost;
            if (best_tree) {
                best_tree->erase();
                delete(best_tree);
            }
            best_tree = tree;
        } else {
            tree->erase();
            delete(tree);
        }
    }
    original->erase();
    delete original;
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
    Tree* bst_tree = nullptr;
    int bst_cst = 1e9;
    int k_max = 160;
    int can_factorize = true;
    for (int k = 0; k < k_max; k++) {
        root->trim();
        if (!can_factorize || Random::integer(5 * k_max) < k_max - k) { // defactorize
            defactorize(root);
            can_factorize = true;
        }
        else { // factorize
            if (!factorize(root)) {
                can_factorize = false;
                continue;
            }
        }
        int root_cst = root->get_cost();
        if (bst_cst > root_cst) {
            bst_cst = root_cst;
            if (bst_tree) {
                bst_tree->erase();
                delete bst_tree;
            }
            bst_tree = root->deep_copy();
        }
    }
    root->erase();
    delete root;
    bst_tree->trim();
    while(factorize(bst_tree))
        bst_tree->trim();
    return bst_tree;
}

// Parameters which can be changed
long double cooling_rate = 0.10;
long double t_max = 100;
long double t_min = 10;
int defactorzie_percent = 30;
int L = 25; // number of iterations per sa epoch
// 

Tree* real_sa(Tree* root) {
    root->trim();
        
    // Tree* bst_tree = nullptr;
    // int bst_cst = 1e9;
    int root_cst = root->get_cost();

    for (long double t = t_max; t > t_min; t = (1 - cooling_rate) * t) {
        for (int i=0;i<L;i++) {
            Tree* neighbour = root->deep_copy();

            neighbour->trim();
            if (Random::integer(101) <= defactorzie_percent) {
                defactorize(neighbour);
            }
            else {
                factorize(neighbour);
            }

            int neighbour_cst = neighbour->get_cost();

            int delta = neighbour_cst - root_cst;

            if (delta < 0) {
                root_cst = neighbour_cst;
                root->erase();
                delete root;
                root = neighbour;
            } else {
                if (Random::integer(1000000000) / 1000000000.0 <= exp(-delta/t)) {
                    root_cst = neighbour_cst;
                    root->erase();
                    delete root;
                    root = neighbour;
                } else {
                    neighbour->erase();
                    delete neighbour;
                }
            }
        }
    }
    root->trim();
    while(factorize(root))
        root->trim();
    return root;
}

int ich_iterations = 15;

Tree* iterated_simulated_annealing(Tree * original) {    
    int mn = 1e9;
    Tree * tmn = nullptr;
    for (int i=1;i<=ich_iterations;i++) {
        Tree *tree = original->deep_copy();
        tree->trim();
        tree = simulated_annealing(tree);
        int tt = tree->get_cost();
        if(mn > tt) {
            if (tmn) {
                tmn->erase();
                delete(tmn);
            }
            tmn = tree;
            mn = tt;
        } else {
            tree->erase();
            delete(tree);
        }
    }
    original->erase();
    delete original;
    return tmn;
}

int isa_iterations = 15;

Tree* iterated_rsa(Tree * original) {    
    int mn = 1e9;
    Tree * tmn = nullptr;
    for (int i=1;i<=isa_iterations;i++) {
        Tree *tree = original->deep_copy();
        tree->trim();
        tree = real_sa(tree);
        int tt = tree->get_cost();
        if(mn > tt) {
            if (tmn) {
                tmn->erase();
                delete(tmn);
            }
            tmn = tree;
            mn = tt;
        } else {
            tree->erase();
            delete(tree);
        }
    }
    original->erase();
    delete original;
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
std::mutex st_lock;

void run_algorithm(std::string formula, Tree * (*algorithm)(Tree *), long double& ttime, long double& score) {
    Tree *tree1 = &Logic::to_tree(formula);
    long double t_start = current_time_ms();
    long double s_start = tree1->get_cost();
    
    tree1 = algorithm(tree1);
    long double t_end = current_time_ms();
    long double s_end = tree1->get_cost();
    
    ttime = t_end - t_start;
    score = improvement_percent(s_start, s_end);

    tree1->erase();
    delete tree1;
}


void iteration(Tree*(*alg)(Tree *), std::vector<std::string> formulas, 
     std::vector<long double> &times, std::vector<long double> &scores) {

    int formula_count = 0;
    long double iteration_time = 0, iteration_score = 0;
    for (auto &formula : formulas) {
        long double crt_time = 0, crt_score = 0;
        run_algorithm(formula, alg, crt_time, crt_score);   
        iteration_time += crt_time;
        iteration_score += crt_score;
        formula_count++;
    }

    iteration_time /= formula_count;
    iteration_score /= formula_count;

    st_lock.lock();

    times.push_back(iteration_time);
    scores.push_back(iteration_score);

    st_lock.unlock();
}

struct ALG{
    std::string time_label;
    std::string score_label;
    std::string best_score_label;
    std::string custom_label;
};

int main(int argc, char* argv[]) {
    
    // load_patterns();
    const int ITERATION_COUNT = 500;
    int thread_count = 6;

    std::vector<ALG> alg_to_test;

    alg_to_test.push_back({
        "        hc time: ",
        "       hc score: ",
        "  hc best score: ",
        "             hc: "
    });
    alg_to_test.push_back({
        "       ihc time: ",
        "      ihc score: ",
        " ihc best score: ",
        "            ihc: "
    });
    alg_to_test.push_back({
        "        sa time: ",
        "       sa score: ",
        "  sa best score: ",
        "             sa: "
    });
    alg_to_test.push_back({
        "       isa time: ",
        "      isa score: ",
        " isa best score: ",
        "            isa: "
    });
    alg_to_test.push_back({
        "       rsa time: ",
        "      rsa score: ",
        " rsa best score: ",
        "            rsa: "
    });
    alg_to_test.push_back({
        "      irsa time: ",
        "     irsa score: ",
        "irsa best score: ",
        "           irsa: "
    });

    std::vector <Tree*(*)(Tree *)> alg;
    alg.push_back(&hill_climbing);
    alg.push_back(&iterated_hc);
    alg.push_back(&simulated_annealing);
    alg.push_back(&iterated_simulated_annealing);
    alg.push_back(&real_sa);
    alg.push_back(&iterated_rsa);

    std::vector<std::vector<long double> > times(alg.size(), std::vector<long double>());
    std::vector<std::vector<long double> > scores(alg.size(), std::vector<long double>());
    std::vector<long double> time_sum(alg.size()), time_mean(alg.size());
    std::vector<long double> score_sum(alg.size()), score_mean(alg.size()), score_square_sum_dif(alg.size()), 
        score_trust_interval_min(alg.size()), score_trust_interval_max(alg.size());
    std::vector<long double> alg_best_score(alg.size());
    std::vector<std::thread> threads;
    std::vector<std::string> formulas;
    
    std::string formula;
    std::ifstream fin(argv[1]);
    while (fin >> formula) {
        formulas.push_back(formula);
    }
    int formula_count = formulas.size();

    for (int a = 0; a < (int)alg.size(); a++) {
        std::cout << "started algorithm #" << a << '\n';
        for (int i = 0; i < ITERATION_COUNT; i++) {
            std::cout << "started iteration #" << i << '\n';

            std::thread t(iteration, alg[a], formulas, std::ref(times[a]), std::ref(scores[a]));
            threads.push_back(std::move(t));

            if (i % thread_count == (thread_count - 1) || i == ITERATION_COUNT - 1) {
                for(auto& thread : threads)
                    thread.join();
                threads.clear();
            }
        }

        for (auto s : scores[a]) {
            score_sum[a] += s;
            alg_best_score[a] = std::max(alg_best_score[a], s);
        }

        for (auto t : times[a]) {
            time_sum[a] += t;
        }
    }

    for (int i=0;i<(int)alg.size();i++) {
        time_mean[i] = time_sum[i] / ITERATION_COUNT / 1000;
        score_mean[i] = score_sum[i] / ITERATION_COUNT;
    }

    int z = 1.96; // 95%
    for (int a = 0; a < (int)alg.size(); a++) {
        for (int i = 0; i < ITERATION_COUNT; i++) {
           score_square_sum_dif[a] += (score_mean[a] - scores[a][i]) * (score_mean[a] - scores[a][i]);
        }
        score_square_sum_dif[a] /= ITERATION_COUNT;
        score_square_sum_dif[a] = sqrt(score_square_sum_dif[a]);
        score_trust_interval_min[a] = score_mean[a] - z * score_square_sum_dif[a] / sqrt(ITERATION_COUNT);
        score_trust_interval_max[a] = score_mean[a] + z * score_square_sum_dif[a] / sqrt(ITERATION_COUNT);
    }

    const auto p1 = std::chrono::system_clock::now();
    long long timestamp_now = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();

    std::ofstream fout("./full_results/full_results" + std::to_string(timestamp_now) + ".txt");

    fout << std::fixed << std::setprecision(6);

    fout << "Dataset: " << argv[1] << '\n';

    fout << "ITERATION_COUNT: " << ITERATION_COUNT << "\n\n";
    fout << "SA params: \n";
    fout << "\tcooling_rate: " << cooling_rate << '\n';
    fout << "\tt_max: " << t_max << '\n';
    fout << "\tt_min: " << t_min << '\n';
    fout << "\tdefactorzie_percent: " << defactorzie_percent << '\n';
    fout << "\tL: " << L << "\n\n";

    fout << "ISA iterations: " << isa_iterations << '\n';
    fout << "ICH iterations: " << ich_iterations << '\n';
    fout << "IHC iterations: " << ihc_iterations << '\n';

    for (int a = 0; a < (int)alg_to_test.size(); a++) {
        fout << alg_to_test[a].custom_label << '\n';
        for (int i = 0; i < ITERATION_COUNT; i++) {
            fout << times[a][i] << ' ';
        }
        fout << '\n';
        for (int i = 0; i < ITERATION_COUNT; i++) {
            fout << scores[a][i] << ' ';
        }
        fout << '\n';
        fout << '\n';
    }

    for (int i = 0; i < (int)alg_to_test.size(); i++) {
        fout << alg_to_test[i].time_label << ' ' << time_mean[i] << '\n';
        std::cout << alg_to_test[i].time_label << ' ' << time_mean[i] << '\n';
    }
    fout << '\n';
    std::cout << '\n';

    for (int i = 0; i < (int)alg_to_test.size(); i++) {
        fout << alg_to_test[i].score_label << ' ' << score_mean[i] << '\n';
        std::cout << alg_to_test[i].score_label << ' ' << score_mean[i] << '\n';
    }
    fout << '\n';
    std::cout << '\n';

    for (int i = 0; i < (int)alg_to_test.size(); i++) {
        fout << alg_to_test[i].best_score_label << ' ' << alg_best_score[i] << '\n';
        std::cout << alg_to_test[i].best_score_label << ' ' << alg_best_score[i] << '\n';
    }
    fout << '\n';
    std::cout << '\n';


    fout << "Standard dev:\n";
    std::cout << "Standard dev:\n";
    for (int i = 0; i < (int)alg_to_test.size(); i++) {
        fout << alg_to_test[i].custom_label << ' ' << score_square_sum_dif[i] << '\n';
        std::cout << alg_to_test[i].custom_label << ' ' << score_square_sum_dif[i] << '\n';
    }
    fout << '\n';
    std::cout << '\n';

    fout << "Trust intervals (c-95):\n";
    std::cout << "Trust intervals (c-95):\n";
    for (int i = 0; i < (int)alg_to_test.size(); i++) {
        fout << alg_to_test[i].custom_label << ' ' << score_trust_interval_min[i] << ' ' << score_trust_interval_max[i] << '\n';
        std::cout << alg_to_test[i].custom_label << ' ' << score_trust_interval_min[i] << ' ' << score_trust_interval_max[i] << '\n';
    }
    fout << '\n';
    std::cout << '\n';

    fout.close();

    return 0;
}
