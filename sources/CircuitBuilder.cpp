#include "../headers/abeai.h"
#include "../headers/debug.h"

void check_circuit(Node* root, int leaf_count) {
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

Circuit& CircuitBuilder::random(int leaf_count, int max_lower_count) {
    std::mt19937 rand(std::chrono::steady_clock::now().time_since_epoch().count());
    std::vector<Node*> leaves(leaf_count);
    for (Node*& leaf : leaves)
        leaf = new Node(INPUT);
    std::vector<std::vector<Node*>> nodes(2);
    int current_level = 0;
    nodes[current_level] = leaves;
    while (nodes[current_level].size() > 1) {
        current_level ^= 1;
        nodes[current_level].clear();
        nodes[current_level].resize((nodes[!current_level].size() + 1) / 2);
        for (Node*& node : nodes[current_level])
            node = new Node(rand() % 2 ? AND : OR);
        std::set<Node*> upper_nodes;
        for (Node* node : nodes[current_level])
            upper_nodes.insert(node);
        for (Node* node : nodes[!current_level]) {
            std::vector<Node*> the_upper_node;
            std::sample(nodes[current_level].begin(), nodes[current_level].end(), std::back_inserter(the_upper_node), 1, rand);
            Node *upper_node = the_upper_node.back();
            upper_node->lower.insert(node);
            node->upper.insert(upper_node);
            if ((int)upper_node->lower.size() == max_lower_count)
                upper_nodes.erase(upper_node);
        }
        for (Node* node : nodes[current_level]) {
            if (node->lower.size() >= 2) continue;
            std::set<Node*> available_nodes;
            for (Node* node : nodes[!current_level])
                available_nodes.insert(node);
            if (!node->lower.empty())
                available_nodes.erase(*(node->lower.begin()));
            const int lower_count = 2 - node->lower.size();
            std::vector<Node*> lower_nodes;
            std::sample(available_nodes.begin(), available_nodes.end(), std::back_inserter(lower_nodes), lower_count, rand);
            for (Node* lower_node : lower_nodes) {
                node->lower.insert(lower_node);
                lower_node->upper.insert(node);
            }
        }
        for (Node* node : nodes[!current_level]) {
            if (node->upper.size() == 1) continue;
            Node *fan_out_node = new Node(FAN_OUT);
            for (Node* upper_node : node->upper) {
                upper_node->lower.erase(node);
                upper_node->lower.insert(fan_out_node);
                fan_out_node->upper.insert(upper_node);
            }
            node->upper.clear();
            node->upper.insert(fan_out_node);
            fan_out_node->lower.insert(node);
        }
    }
    check_circuit(nodes[current_level][0], leaf_count);
    return *(new Circuit(nodes[current_level][0], leaves));
}

std::vector<int> random_partition(int value, int parts) {
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

Circuit& CircuitBuilder::random(int height, int node_count, int leaf_count) {
    NodeType node_types[] = {AND, OR, FAN_OUT};
    std::mt19937 rand(std::chrono::steady_clock::now().time_since_epoch().count());
    assert(height >= 4);
    assert(leaf_count >= 4);

    std::vector<int> size_of_level(height + 1);
    node_count -= size_of_level[0] = 1;
    node_count -= size_of_level[height] = leaf_count;
    node_count -= size_of_level[height - 1] = rand() % ((leaf_count - 2) / 2) + 3;
    assert(node_count >= height - 2);

    auto partition = random_partition(node_count, height - 2);
    for (int i = 1; i < height - 1; i++)
        size_of_level[i] = partition[i - 1];
    node_count = 0;

    std::vector<std::vector<Node*>> nodes_on_level(height + 1);
    for (const int i : {0, 1, height - 1})
        for (int j = 0; j < size_of_level[i]; j++)
            nodes_on_level[i].push_back(new Node(node_types[rand() % 2]));
    nodes_on_level[height - 1].front()->type = FAN_OUT;
    nodes_on_level[height - 1].back()->type = FAN_OUT;
    for (int i = 2; i < height - 1; i++)
        for (int j = 0; j < size_of_level[i]; j++)
            nodes_on_level[i].push_back(new Node(node_types[rand() % 3]));
    for (int j = 0; j < size_of_level[height]; j++)
        nodes_on_level[height].push_back(new Node(INPUT));

    for (int i = 1; i <= height; i++) {
        Node *node = nodes_on_level[i][rand() % size_of_level[i]];
        Node *upper_node = nodes_on_level[i - 1][i == height ? 0 : rand() % size_of_level[i - 1]];
        node->upper.insert(upper_node);
        upper_node->lower.insert(node);
    }

    std::set<Node*> lower_nodes;
    auto add_lower_nodes = [&](Node* node) {
        const int lower_count = node->lower.size();
        const int lower_count_target = node->type == FAN_OUT ? 1 : 2;
        if (lower_count >= lower_count_target) return;
        const int lower_count_delta = lower_count_target - lower_count;
        std::vector<Node*> already_used;
        for (Node* lower_node : node->lower)
            if (lower_nodes.count(lower_node)) {
                already_used.push_back(lower_node);
                lower_nodes.erase(lower_node);
            }
        std::vector<Node*> sampled_nodes;
        std::sample(lower_nodes.begin(), lower_nodes.end(), std::back_inserter(sampled_nodes), lower_count_delta, rand);
        for (Node* lower_node : sampled_nodes) {
            node->lower.insert(lower_node);
            lower_node->upper.insert(node);
            if (lower_node->type != FAN_OUT && lower_node->upper.size() == 1)
                lower_nodes.erase(lower_node);
        }
        for (Node* lower_node : already_used)
            lower_nodes.insert(lower_node);
    };

    for (Node* node : nodes_on_level[height])
        if (node->upper.size() != 1)
            lower_nodes.insert(node);
    add_lower_nodes(nodes_on_level[height - 1].front());
    add_lower_nodes(nodes_on_level[height - 1].back());
    for (Node* node : nodes_on_level[height - 1])
        add_lower_nodes(node);
    for (int i = 0; i <= height; i++)
        for (Node* node : nodes_on_level[i])
            if (!(node->type != FAN_OUT && node->upper.size() == 1))
                lower_nodes.insert(node);
    for (int i = 0; i < height - 1; i++) {
        for (Node* node : nodes_on_level[i])
            lower_nodes.erase(node);
        for (Node* node : nodes_on_level[i])
            add_lower_nodes(node);
    }

    std::set<Node*> upper_nodes;
    auto add_upper_nodes = [&](Node* node) {
        const int upper_count = node->upper.size();
        const int upper_count_target = node->type == FAN_OUT ? rand() % (upper_nodes.size() - 1) + 2 : 1;
        if (upper_count >= upper_count_target) return;
        const int upper_count_delta = upper_count_target - upper_count;
        std::vector<Node*> already_used;
        for (Node* upper_node : node->upper)
            if (upper_nodes.count(upper_node)) {
                already_used.push_back(upper_node);
                upper_nodes.erase(upper_node);
            }
        std::vector<Node*> sampled_nodes;
        std::sample(upper_nodes.begin(), upper_nodes.end(), std::back_inserter(sampled_nodes), upper_count_delta, rand);
        for (Node* upper_node : sampled_nodes) {
            node->upper.insert(upper_node);
            upper_node->lower.insert(node);
        }
        for (Node* upper_node : already_used)
            upper_nodes.insert(upper_node);
    };

    for (int i = 0; i <= height; i++)
        for (Node* node : nodes_on_level[i])
            if (node->type != FAN_OUT)
                upper_nodes.insert(node);
    for (int i = height; i > 0; i--) {
        for (Node* node : nodes_on_level[i])
            upper_nodes.erase(node);
        for (Node* node : nodes_on_level[i])
            add_upper_nodes(node);
    }
    check_circuit(nodes_on_level[0][0], leaf_count);
    return *(new Circuit(nodes_on_level[0][0], nodes_on_level[height]));
}

Circuit& CircuitBuilder::from(const std::vector<NodeType>& types, const std::vector<std::pair<int, int>>& edges) {
    std::vector<Node*> nodes;
    for (const NodeType type : types)
        nodes.push_back(new Node(type));
    for (const auto& [node1, node2] : edges) {
        nodes[node1]->lower.insert(nodes[node2]);
        nodes[node2]->upper.insert(nodes[node1]);
    }
    std::vector<Node*> leaves;
    Node* root = nullptr;
    for (Node* node : nodes) {
        if (node->lower.empty())
            leaves.push_back(node);
        if (node->upper.empty()){
            assert(root == nullptr);
            root = node;
        }
    }
    return *(new Circuit(root, leaves));
}
