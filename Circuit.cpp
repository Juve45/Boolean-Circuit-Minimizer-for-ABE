#include <functional>
#include <iostream>
#include <queue>
#include <map>
#include <algorithm>
#include <assert.h>
#include "abeai.h"
#include "debug.h"

Circuit& Circuit::from(const std::vector<NodeType>& types, const std::vector<std::pair<int, int>>& edges) {
    std::vector<Node*> nodes;
    for (const NodeType type : types)
        nodes.push_back(new Node(type));
    for (const auto& [node1, node2] : edges) {
        nodes[node1]->bottom.insert(nodes[node2]);
        nodes[node2]->top.insert(nodes[node1]);
    }
    std::vector<Node*> leaves;
    for (Node* node : nodes)
        leaves.push_back(node);
    return *(new Circuit(nodes[0], leaves));
}

// https://csacademy.com/app/graph_editor/
void Circuit::print() {
    auto stringify = [&](NodeType type) {
        if (type == AND) return "AND";
        if (type == OR) return "OR";
        if (type == FAN_OUT) return "FAN_OUT";
        if (type == INPUT) return "INPUT";
        return "";
    };
    std::set<Node*> visited;
    std::function<void(Node*)> dfs = [&](Node* node) {
        visited.insert(node);
        for (Node* bottomNode : node->bottom) {
            std::cout << node->id << '-' << stringify(node->type) << ' ';
            std::cout << bottomNode->id << '-' << stringify(bottomNode->type) << '\n';
            if (!visited.count(bottomNode))
                dfs(bottomNode);
        }
    };
    dfs(root);
}

Circuit& Circuit::copy() {
    std::vector<Node*> newLeaves;
    std::map<Node*, Node*> oldToNew;
    std::function<void(Node*)> dfs = [&](Node* node) {
        oldToNew[node] = new Node(node->type);
        for (Node* bottomNode : node->bottom)
            if (!oldToNew.count(bottomNode))
                dfs(bottomNode);
        if (node->bottom.empty())
            newLeaves.push_back(oldToNew[node]);
    };
    dfs(root);
    for (const auto& [oldNode, newNode] : oldToNew) {
        for (Node* topNode : oldNode->top)
            newNode->top.insert(oldToNew[topNode]);
        for (Node* bottomNode : oldNode->bottom)
            newNode->bottom.insert(oldToNew[bottomNode]);
    }
    return *(new Circuit(oldToNew[root], newLeaves));
}

int Circuit::eval() {
    std::map<int, int> nodeTopCisited = std::map<int, int>(); 
    std::map<int, int> nodeValue = std::map<int, int>(); 

    std::queue<std::pair<Node*, int> > nodes;
    nodes.push({this->root, 1});

    int ans = 0;

    while (!nodes.empty()){
        auto &[node, value] = nodes.front();
        nodes.pop();

        for (Node* nextNode : node->bottom) {
            nodeTopCisited[nextNode->id]++;
            nodeValue[nextNode->id] += value;
            if (nodeTopCisited[nextNode->id] == nextNode->top.size()) {
                nodes.push({nextNode, nodeValue[nextNode->id]});
            }
        }

        if (node->type == NodeType::INPUT) {
            ans += value;
        }
    }

    return ans;
}

void Circuit::replaceSubCircuit(const SubCircuit& found, const SubCircuit& toReplace) {
    assert(found.topEdges.size() == toReplace.topEdges.size());
    assert(found.bottomEdges.size() == toReplace.bottomEdges.size());

    std::set<Node*> toDelete;
    std::set<Node*> deleteEndNode;

    // Replace top nodes
    for (int i=0; i<found.topEdges.size();i++) {
        // TO DO if root
        Edge* foundEdge = found.topEdges[i];
        Edge* toReplaceEdge = toReplace.topEdges[i];
        Node* outsideOldNode = foundEdge->top;
        Node* insideOldNode = foundEdge->bottom;
        toDelete.insert(insideOldNode);

        Node* insideNewNode = toReplaceEdge->bottom;
        outsideOldNode->bottom.erase(insideOldNode);
        outsideOldNode->bottom.insert(insideNewNode);
        insideNewNode->top.insert(outsideOldNode);
    }

    // Replace bottom nodes
    for (int i=0; i<found.bottomEdges.size();i++) {
        Edge* foundEdge = found.bottomEdges[i];
        Edge* toReplaceEdge = toReplace.bottomEdges[i];
        Node* outsideOldNode = foundEdge->bottom;
        Node* insideOldNode = foundEdge->top;
        deleteEndNode.insert(insideOldNode);

        Node* insideNewNode = toReplaceEdge->top;
        outsideOldNode->top.erase(insideOldNode);
        outsideOldNode->top.insert(insideNewNode);
        insideNewNode->bottom.insert(outsideOldNode);
    }

    // delete old nodes
    std::queue<Node*> toDeleteQueue;
    std::set<Node*> visitedNodes;

    for (Node* node : toDelete) {
        visitedNodes.insert(node);
        toDeleteQueue.push(node);
    }

    while (!toDeleteQueue.empty()) {
        Node* node = toDeleteQueue.front();
        toDeleteQueue.pop();
        if (!deleteEndNode.count(node)) {
            for (Node* nextNode : node->bottom) {
                if (visitedNodes.count(nextNode)) {
                    continue;
                }
                visitedNodes.insert(nextNode);
                toDeleteQueue.push(nextNode);
            }
        }
        delete node;
    }
}


SubCircuit::SubCircuit() {

}

SubCircuit::SubCircuit(const Circuit& circuit) {

    dbg(circuit.leaves);
    dbg(circuit.root);

    for(auto leaf : circuit.leaves) 
        this->bottomEdges.push_back(new Edge(leaf, NULL));
    this->topEdges.push_back(new Edge(NULL, circuit.root));
}




ostream& operator<<(ostream& out, Node * node) { 

    out << "Node[" << node->id << "]";
    return out; 
}