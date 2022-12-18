#include <algorithm>
#include <iostream>
#include <random>
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

CircuitBuilder::CircuitBuilder(int height, int nodeCount, int leafCount) :
    height(height),
    nodeCount(nodeCount),
    leafCount(leafCount) { }

// not finished
Circuit& CircuitBuilder::build() {
    std::vector<int> sizeOfLevel(height + 1);
    sizeOfLevel[0] = 1;
    auto partition = randomPartition(nodeCount - leafCount - 1, height - 1);
    for (int i = 1; i < height; i++)
        sizeOfLevel[i] = partition[i - 1];
    sizeOfLevel[height] = leafCount;

    std::mt19937 rand(std::chrono::steady_clock::now().time_since_epoch().count());
    NodeType nodeTypes[] = {AND, OR, FAN_OUT};
    std::vector nodesOnLevel(height + 1, std::vector<Node*>());
    for (int i = 0; i <= height; i++)
        for (int j = 0; j < sizeOfLevel[i]; j++)
            nodesOnLevel[i].push_back(new Node(i == height ? INPUT : nodeTypes[rand() % (i < 2 ? 2 : 3)]));

    std::vector<Node*> topNodes;
    for (int i = 0; i <= height; i++) {
        for (Node* node : nodesOnLevel[i]) {
            const int topCount = node->type == FAN_OUT ? topNodes.size() : i == 0 ? 0 : 1;
            for (int j = 0; j < topCount; j++) {
                std::vector<Node*> sampledNodes;
                std::sample(topNodes.begin(), topNodes.end(), std::back_inserter(sampledNodes), topCount, rand);
                for (Node* topNode : sampledNodes)
                    node->top.insert(topNode);
            }
        }
        for (Node* node : nodesOnLevel[i])
            topNodes.push_back(node);
    }
    std::vector<Node*> bottomNodes;
    for (int i = height; i >= 0; i--) {
        for (Node* node : nodesOnLevel[i]) {
            const int bottomCount = node->type == FAN_OUT ? bottomNodes.size() : i == height ? 0 : 2;
            for (int j = 0; j < bottomCount; j++) {
                std::vector<Node*> sampledNodes;
                std::sample(bottomNodes.begin(), bottomNodes.end(), std::back_inserter(sampledNodes), bottomCount, rand);
                for (Node* bottomNode : sampledNodes)
                    node->bottom.insert(bottomNode);
            }
        }
        for (Node* node : nodesOnLevel[i])
            bottomNodes.push_back(node);
    }
    return *(new Circuit(nodesOnLevel[0][0], nodesOnLevel[height]));
}

int main() {
    CircuitBuilder builder(10, 30, 5);
    Circuit circuit = builder.build();
    return 0;
}
