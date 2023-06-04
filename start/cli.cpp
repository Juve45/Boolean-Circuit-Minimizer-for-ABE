#include "../headers/abeai.h"
#include "../headers/debug.h"

bool has_or(std::vector<Tree*> nodes) {
    for (Tree* i : nodes)
        if (i->type == OR)
            return true;
    return false;
}

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
    if (and_nodes.size() == 0) return nullptr;
    const int i = Random::integer(and_nodes.size());
    return and_nodes[i];
}

bool factorize(Tree* root) {
    std::vector<std::vector<Tree*>> factorizable = Factorizer::reduce(root, OR);
    if (factorizable.empty()) return false;
    const int c = Random::integer(factorizable.size());
    const auto [f1, f2] = Random::two_integers(factorizable[c].size());
    Factorizer::factorize(factorizable[c][f1], factorizable[c][f2]);
    return true;
}

void defactorize(Tree* root) {
    Tree *and_node = get_random_and(root);
    if (!and_node) return;
    std::vector<Tree*> or_nodes;
    for (Tree* i : and_node->children)
        if (i->type == OR)
            or_nodes.push_back(i);
    const int f1 = Random::integer(or_nodes.size());
    int f2 = Random::integer(and_node->children.size());
    while (or_nodes[f1] == and_node->children[f2])
        f2 = Random::integer(and_node->children.size());
    Factorizer::defactorize(or_nodes[f1], and_node->children[f2]);
}

Tree* hill_climbing(Tree* t) {
    while (true) {
        t->trim();
        std::vector<std::vector<Tree*>> factorizable = Factorizer::reduce(t, OR);
        if (factorizable.empty()) break;
        const int c = Random::integer(factorizable.size());
        const auto [f1, f2] = Random::two_integers(factorizable[c].size());
        Factorizer::factorize(factorizable[c][f1], factorizable[c][f2]);
    }
    return t;
}

Tree* iterated_hill_climbing(Tree* original) {
    int best_cost = 1e9;
    Tree *best_tree = nullptr;
    const int runs = 60;
    for (int i = 0; i < runs; i++) {
        Tree *tree = original->deep_copy();
        tree->trim();
        hill_climbing(tree);
        const int cost = tree->get_cost();
        if (cost < best_cost) {
            best_cost = cost;
            if (best_tree) {
                best_tree->erase();
                delete best_tree;
            }
            best_tree = tree;
        }
        else {
            tree->erase();
            delete tree;
        }
    }
    original->erase();
    delete original;
    return best_tree;
}

Tree* simulated_annealing(Tree* root) {
    Tree *bst_tree = nullptr;
    int bst_cst = 1e9;
    bool can_factorize = true;
    const int k_max = 160;
    for (int k = 0; k < k_max; k++) {
        root->trim();
        if (!can_factorize || Random::integer(5 * k_max) < k_max - k) {
            defactorize(root);
            can_factorize = true;
        }
        else if (!factorize(root)) {
            can_factorize = false;
            continue;
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
    while (factorize(bst_tree))
        bst_tree->trim();
    return bst_tree;
}

Tree* iterated_simulated_annealing(Tree* original) {
    int mn = 1e9;
    Tree *tmn = nullptr;
    for (int i = 0; i < 8; i++) {
        Tree *tree = original->deep_copy();
        tree->trim();
        tree = simulated_annealing(tree);
        const int tt = tree->get_cost();
        if (mn > tt) {
            if (tmn) {
                tmn->erase();
                delete tmn;
            }
            tmn = tree;
            mn = tt;
        }
        else {
            tree->erase();
            delete tree;
        }
    }
    original->erase();
    delete original;
    return tmn;
}

Tree* custom_heuristic(Tree* root) {
    const long double cooling_rate = 0.15;
    const long double t_max = 200;
    const long double t_min = 10;
    const int defactorzie_percent = 30;
    const int L = 15;

    root->trim();
    int root_cst = root->get_cost();
    for (long double t = t_max; t > t_min; t = (1 - cooling_rate) * t) {
        for (int i = 0; i < L; i++) {
            Tree *neighbor = root->deep_copy();
            neighbor->trim();
            if (Random::integer(101) <= defactorzie_percent)
                defactorize(neighbor);
            else
                factorize(neighbor);
            const int neighbor_cst = neighbor->get_cost();
            const int delta = neighbor_cst - root_cst;
            if (delta < 0) {
                root_cst = neighbor_cst;
                root->erase();
                delete root;
                root = neighbor;
            }
            else if (Random::integer(1e9) / 1e9 <= exp(-delta / t)) {
                root_cst = neighbor_cst;
                root->erase();
                delete root;
                root = neighbor;
            }
            else {
                neighbor->erase();
                delete neighbor;
            }
        }
    }
    root->trim();
    while (factorize(root))
        root->trim();
    return root;
}

Tree* iterated_custom_heuristic(Tree* original) {
    int mn = 1e9;
    Tree *tmn = nullptr;
    for (int i = 0; i < 8; i++) {
        Tree *tree = original->deep_copy();
        tree->trim();
        tree = custom_heuristic(tree);
        const int tt = tree->get_cost();
        if (mn > tt) {
            if (tmn) {
                tmn->erase();
                delete tmn;
            }
            tmn = tree;
            mn = tt;
        }
        else {
            tree->erase();
            delete tree;
        }
    }
    original->erase();
    delete original;
    return tmn;
}

uint64_t current_time_ms() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

long double improvement_percent(int old_val, int new_val) {
    return std::max((long double)0, old_val == new_val ? (long double)0 : ceil((old_val - new_val) * (long double)100 / old_val));
}

int main(int argc, char* argv[]) {
    assert(argc == 3);
    const std::string algorithm(argv[1]);
    const std::string formula(argv[2]);

    Tree *tree = &Logic::to_tree(formula);
    long double t_start = current_time_ms();
    long double s_start = tree->get_cost();

    auto function = hill_climbing;
    if (algorithm == "hc") function = hill_climbing;
    if (algorithm == "ihc") function = iterated_hill_climbing;
    if (algorithm == "sa") function = simulated_annealing;
    if (algorithm == "isa") function = iterated_simulated_annealing;
    if (algorithm == "ch") function = custom_heuristic;
    if (algorithm == "ich") function = iterated_custom_heuristic;
    tree = function(tree);

    long double t_end = current_time_ms();
    long double s_end = tree->get_cost();

    std::cout << "improvement: " << improvement_percent(s_start, s_end) << "%\n";
    std::cout << "time: " << t_end - t_start << " ms\n";
    std::cout << "formula: " << Logic::to_formula(*tree) << '\n';
    return 0;
}
