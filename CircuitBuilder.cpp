#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include "abeai.h"

void check_circuit(Node* root, int leaf_count) {
    std::set<Node*> nodes;
    std::function<void(Node*)> dfs = [&](Node* node) {
        nodes.insert(node);
        for (Node* bottom_node : node->bottom)
            if (!nodes.count(bottom_node))
                dfs(bottom_node);
    };
    dfs(root);
    int real_leaf_count = 0;
    for (Node* node : nodes) {
        real_leaf_count += node->bottom.empty();
        if (node->type == INPUT && node->top.size() != 1)
            std::cerr << "ERROR: input.top_count = " << node->top.size() << '\n';
        else if (node->type == INPUT && node->bottom.size() != 0)
            std::cerr << "ERROR: input.bottom_count = " << node->bottom.size() << '\n';
        else if (node->type == FAN_OUT && node->top.size() < 2)
            std::cerr << "ERROR: fan_out.top_count = " << node->top.size() << '\n';
        else if (node->type == FAN_OUT && node->bottom.size() != 1)
            std::cerr << "ERROR: fan_out.bottom_count = " << node->bottom.size() << '\n';
        else if ((node->type == AND || node->type == OR) && !((node == root && node->top.size() == 0) || (node != root && node->top.size() == 1)))
            std::cerr << "ERROR: and_or.top_count = " << node->top.size() << '\n';
        else if ((node->type == AND || node->type == OR) && node->bottom.size() < 2)
            std::cerr << "ERROR: and_or.bottom_count = " << node->bottom.size() << '\n';
    }
    if (real_leaf_count != leaf_count)
        std::cerr << "ERROR: more leafs than expected\n";
}

Circuit& CircuitBuilder::random(int leaf_count, int max_bottom_count) {
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
        std::set<Node*> top_nodes;
        for (Node* node : nodes[current_level])
            top_nodes.insert(node);
        for (Node* node : nodes[!current_level]) {
            std::vector<Node*> the_top_node;
            std::sample(nodes[current_level].begin(), nodes[current_level].end(), std::back_inserter(the_top_node), 1, rand);
            Node *top_node = the_top_node.back();
            top_node->bottom.insert(node);
            node->top.insert(top_node);
            if ((int)top_node->bottom.size() == max_bottom_count)
                top_nodes.erase(top_node);
        }
        for (Node* node : nodes[current_level]) {
            if (node->bottom.size() >= 2) continue;
            std::set<Node*> available_nodes;
            for (Node* node : nodes[!current_level])
                available_nodes.insert(node);
            if (!node->bottom.empty())
                available_nodes.erase(*(node->bottom.begin()));
            const int bottom_count = 2 - node->bottom.size();
            std::vector<Node*> bottom_nodes;
            std::sample(available_nodes.begin(), available_nodes.end(), std::back_inserter(bottom_nodes), bottom_count, rand);
            for (Node* bottom_node : bottom_nodes) {
                node->bottom.insert(bottom_node);
                bottom_node->top.insert(node);
            }
        }
        for (Node* node : nodes[!current_level]) {
            if (node->top.size() == 1) continue;
            Node *fan_out_node = new Node(FAN_OUT);
            for (Node* top_node : node->top) {
                top_node->bottom.erase(node);
                top_node->bottom.insert(fan_out_node);
                fan_out_node->top.insert(top_node);
            }
            node->top.clear();
            node->top.insert(fan_out_node);
            fan_out_node->bottom.insert(node);
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
        Node *top_node = nodes_on_level[i - 1][i == height ? 0 : rand() % size_of_level[i - 1]];
        node->top.insert(top_node);
        top_node->bottom.insert(node);
    }

    std::set<Node*> bottom_nodes;
    auto add_bottom_nodes = [&](Node* node) {
        const int bottom_count = node->bottom.size();
        const int bottom_count_target = node->type == FAN_OUT ? 1 : 2;
        if (bottom_count >= bottom_count_target) return;
        const int bottom_count_delta = bottom_count_target - bottom_count;
        std::vector<Node*> already_used;
        for (Node* bottom_node : node->bottom)
            if (bottom_nodes.count(bottom_node)) {
                already_used.push_back(bottom_node);
                bottom_nodes.erase(bottom_node);
            }
        std::vector<Node*> sampled_nodes;
        std::sample(bottom_nodes.begin(), bottom_nodes.end(), std::back_inserter(sampled_nodes), bottom_count_delta, rand);
        for (Node* bottom_node : sampled_nodes) {
            node->bottom.insert(bottom_node);
            bottom_node->top.insert(node);
            if (bottom_node->type != FAN_OUT && bottom_node->top.size() == 1)
                bottom_nodes.erase(bottom_node);
        }
        for (Node* bottom_node : already_used)
            bottom_nodes.insert(bottom_node);
    };

    for (Node* node : nodes_on_level[height])
        if (node->top.size() != 1)
            bottom_nodes.insert(node);
    add_bottom_nodes(nodes_on_level[height - 1].front());
    add_bottom_nodes(nodes_on_level[height - 1].back());
    for (Node* node : nodes_on_level[height - 1])
        add_bottom_nodes(node);
    for (int i = 0; i <= height; i++)
        for (Node* node : nodes_on_level[i])
            if (!(node->type != FAN_OUT && node->top.size() == 1))
                bottom_nodes.insert(node);
    for (int i = 0; i < height - 1; i++) {
        for (Node* node : nodes_on_level[i])
            bottom_nodes.erase(node);
        for (Node* node : nodes_on_level[i])
            add_bottom_nodes(node);
    }

    std::set<Node*> top_nodes;
    auto add_top_nodes = [&](Node* node) {
        const int top_count = node->top.size();
        const int top_count_target = node->type == FAN_OUT ? rand() % (top_nodes.size() - 1) + 2 : 1;
        if (top_count >= top_count_target) return;
        const int top_count_delta = top_count_target - top_count;
        std::vector<Node*> already_used;
        for (Node* top_node : node->top)
            if (top_nodes.count(top_node)) {
                already_used.push_back(top_node);
                top_nodes.erase(top_node);
            }
        std::vector<Node*> sampled_nodes;
        std::sample(top_nodes.begin(), top_nodes.end(), std::back_inserter(sampled_nodes), top_count_delta, rand);
        for (Node* top_node : sampled_nodes) {
            node->top.insert(top_node);
            top_node->bottom.insert(node);
        }
        for (Node* top_node : already_used)
            top_nodes.insert(top_node);
    };

    for (int i = 0; i <= height; i++)
        for (Node* node : nodes_on_level[i])
            if (node->type != FAN_OUT)
                top_nodes.insert(node);
    for (int i = height; i > 0; i--) {
        for (Node* node : nodes_on_level[i])
            top_nodes.erase(node);
        for (Node* node : nodes_on_level[i])
            add_top_nodes(node);
    }
    check_circuit(nodes_on_level[0][0], leaf_count);
    return *(new Circuit(nodes_on_level[0][0], nodes_on_level[height]));
}

Circuit& CircuitBuilder::from(const std::vector<NodeType>& types, const std::vector<std::pair<int, int>>& edges) {
    std::vector<Node*> nodes;
    for (const NodeType type : types)
        nodes.push_back(new Node(type));
    for (const auto& [node1, node2] : edges) {
        nodes[node1]->bottom.insert(nodes[node2]);
        nodes[node2]->top.insert(nodes[node1]);
    }
    std::vector<Node*> leaves;
    for (Node* node : nodes)
        if (node->bottom.empty())
            leaves.push_back(node);
    return *(new Circuit(nodes[0], leaves));
}
