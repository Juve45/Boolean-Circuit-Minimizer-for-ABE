#include <random>
#include <chrono>
#include <algorithm>
#include "abeai.h"

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

// TODO
Circuit& CircuitBuilder::build() {
    std::vector<int> size_of_level(height + 1);
    size_of_level[0] = 1;
    auto partition = random_partition(node_count - leaf_count - 1, height - 1);
    for (int i = 1; i < height; i++)
        size_of_level[i] = partition[i - 1];
    size_of_level[height] = leaf_count;

    std::mt19937 rand(std::chrono::steady_clock::now().time_since_epoch().count());
    NodeType node_types[] = {AND, OR, FAN_OUT};
    std::vector<std::vector<Node*>> nodes_on_level(height + 1);
    for (int i = 0; i <= height; i++)
        for (int j = 0; j < size_of_level[i]; j++) {
            const NodeType type = i == height ? INPUT : node_types[rand() % (i ? 3 : 2)];
            nodes_on_level[i].push_back(new Node(type));
        }

    // making sure the final number of levels is actually going to be `height + 1`
    for (int i = 1; i <= height; i++) {
        Node *node = nodes_on_level[i][rand() % nodes_on_level[i].size()];
        Node *top_node = nodes_on_level[i - 1][rand() % nodes_on_level[i - 1].size()];
        node->top.insert(top_node);
    }

    std::vector<Node*> top_nodes;
    for (int i = 0; i <= height; i++) {
        for (Node* node : nodes_on_level[i]) {
            const int top_count = (node->type == FAN_OUT || node->type == INPUT ? rand() % top_nodes.size() + 1 : i ? 1 : 0) - node->top.size();
            std::sample(top_nodes.begin(), top_nodes.end(), std::inserter(node->top, node->top.end()), top_count, rand);
            for (Node* top_node : node->top)
                top_node->bottom.insert(node);
        }
        for (Node* node : nodes_on_level[i])
            top_nodes.push_back(node);
    }

    std::vector<Node*> new_leaves;
    for (int i = 1; i <= height; i++)
        for (Node* node : nodes_on_level[i])
            if (node->bottom.empty())
                new_leaves.push_back(node);
    return *(new Circuit(nodes_on_level[0][0], new_leaves));
}
