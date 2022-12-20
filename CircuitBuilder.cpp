#include <random>
#include <chrono>
#include <algorithm>
#include "abeai.h"

std::vector<int> randomPartition(int value, int parts) {
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
    std::vector<int> sizeOfLevel(height + 1);
    sizeOfLevel[0] = 1;
    auto partition = randomPartition(nodeCount - leafCount - 1, height - 1);
    for (int i = 1; i < height; i++)
        sizeOfLevel[i] = partition[i - 1];
    sizeOfLevel[height] = leafCount;

    std::mt19937 rand(std::chrono::steady_clock::now().time_since_epoch().count());
    NodeType nodeTypes[] = {AND, OR, FAN_OUT};
    std::vector<std::vector<Node*>> nodesOnLevel(height + 1);
    for (int i = 0; i <= height; i++)
        for (int j = 0; j < sizeOfLevel[i]; j++) {
            const NodeType type = i == height ? INPUT : nodeTypes[rand() % (i ? 3 : 2)];
            nodesOnLevel[i].push_back(new Node(type));
        }

    // making sure the final number of levels is actually going to be `height + 1`
    for (int i = 1; i <= height; i++) {
        Node *node = nodesOnLevel[i][rand() % nodesOnLevel[i].size()];
        Node *topNode = nodesOnLevel[i - 1][rand() % nodesOnLevel[i - 1].size()];
        node->top.insert(topNode);
    }

    std::vector<Node*> topNodes;
    for (int i = 0; i <= height; i++) {
        for (Node* node : nodesOnLevel[i]) {
            const int topCount = (node->type == FAN_OUT || node->type == INPUT ? rand() % topNodes.size() + 1 : i ? 1 : 0) - node->top.size();
            std::sample(topNodes.begin(), topNodes.end(), std::inserter(node->top, node->top.end()), topCount, rand);
            for (Node* topNode : node->top)
                topNode->bottom.insert(node);
        }
        for (Node* node : nodesOnLevel[i])
            topNodes.push_back(node);
    }

    std::vector<Node*> newLeaves;
    for (int i = 1; i <= height; i++)
        for (Node* node : nodesOnLevel[i])
            if (node->bottom.empty())
                newLeaves.push_back(node);
    return *(new Circuit(nodesOnLevel[0][0], newLeaves));
}
