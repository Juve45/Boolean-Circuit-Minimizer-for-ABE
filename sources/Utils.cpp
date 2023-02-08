#include "../headers/abeai.h"
#include "../headers/debug.h"

void Utils::check_circuit(Node* root, int leaf_count) {
    std::set<Node*> nodes;
    std::function<void(Node*)> dfs = [&](Node* node) {
        nodes.insert(node);
        for (Node* lower_node : node->lower)
            if (!nodes.count(lower_node))
                dfs(lower_node);
    };
    dfs(root);
    int real_leaf_count = 0;
    for (Node* node : nodes) {
        real_leaf_count += node->lower.empty();
        if (node->type == INPUT && node->upper.size() != 1)
            std::cerr << "ERROR: input.upper_count = " << node->upper.size() << '\n';
        else if (node->type == INPUT && node->lower.size() != 0)
            std::cerr << "ERROR: input.lower_count = " << node->lower.size() << '\n';
        else if (node->type == FAN_OUT && node->upper.size() < 2)
            std::cerr << "ERROR: fan_out.upper_count = " << node->upper.size() << '\n';
        else if (node->type == FAN_OUT && node->lower.size() != 1)
            std::cerr << "ERROR: fan_out.lower_count = " << node->lower.size() << '\n';
        else if ((node->type == AND || node->type == OR) && !((node == root && node->upper.size() == 0) || (node != root && node->upper.size() == 1)))
            std::cerr << "ERROR: and_or.upper_count = " << node->upper.size() << '\n';
        else if ((node->type == AND || node->type == OR) && node->lower.size() < 2)
            std::cerr << "ERROR: and_or.lower_count = " << node->lower.size() << '\n';
    }
    if (real_leaf_count != leaf_count)
        std::cerr << "ERROR: more leafs than expected\n";
}

std::vector<int> Utils::random_partition(int value, int parts) {
    std::mt19937 rand(std::chrono::steady_clock::now().time_since_epoch().count());
    std::vector<int> partition(parts);
    for (int i = 0; i < parts - 1; i++) {
        partition[i] = rand() % (value - parts + i + 1) + 1;
        value -= partition[i];
    }
    partition[parts - 1] = value;
    std::shuffle(partition.begin(), partition.end(), rand);
    return partition;
}
