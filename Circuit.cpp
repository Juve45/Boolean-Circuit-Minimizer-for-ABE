#include <algorithm>
#include <cassert>
#include <fstream>
#include <functional>
#include <map>
#include <queue>
#include "abeai.h"

std::ostream& operator<<(std::ostream& out, const Circuit& circuit) {
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
        for (Node* bottom_node : node->bottom) {
            out << *node << '-' << stringify(node->type) << ' ';
            out << *bottom_node << '-' << stringify(bottom_node->type) << '\n';
            if (!visited.count(bottom_node))
                dfs(bottom_node);
        }
    };
    dfs(circuit.root);
    out << "root: " << *circuit.root << '\n';
    out << "leaves: ";
    for (Node* leaf : circuit.leaves)
        out << *leaf << ' ';
    out << '\n';
    return out;
}

Circuit& Circuit::copy() {
    std::vector<Node*> new_leaves;
    std::map<Node*, Node*> old_to_new;
    std::function<void(Node*)> dfs = [&](Node* node) {
        old_to_new[node] = new Node(node->type);
        for (Node* bottom_node : node->bottom)
            if (!old_to_new.count(bottom_node))
                dfs(bottom_node);
        if (node->bottom.empty())
            new_leaves.push_back(old_to_new[node]);
    };
    dfs(root);
    for (const auto& [old_node, new_node] : old_to_new) {
        for (Node* top_node : old_node->top)
            new_node->top.insert(old_to_new[top_node]);
        for (Node* bottom_node : old_node->bottom)
            new_node->bottom.insert(old_to_new[bottom_node]);
    }
    return *(new Circuit(old_to_new[root], new_leaves));
}

int Circuit::eval() {
    std::map<int, int> node_top_visited = std::map<int, int>();
    std::map<int, int> node_value = std::map<int, int>();

    std::queue<std::pair<Node*, int> > nodes;
    nodes.push({this->root, 1});

    int ans = 0;

    while (!nodes.empty()){
        auto &[node, value] = nodes.front();
        nodes.pop();

        for (Node* next_node : node->bottom) {
            node_top_visited[next_node->id]++;
            node_value[next_node->id] += value;
            if (node_top_visited[next_node->id] == next_node->top.size()) {
                nodes.push({next_node, node_value[next_node->id]});
            }
        }

        if (node->type == NodeType::INPUT) {
            ans += value;
        }
    }

    return ans;
}

void Circuit::replace_subcircuit(const SubCircuit& found, const SubCircuit& to_replace) {
    assert(found.top_edges.size() == to_replace.top_edges.size());
    assert(found.bottom_edges.size() == to_replace.bottom_edges.size());

    std::set<Node*> to_delete;
    std::set<Node*> delete_end_node;

    // Replace top nodes
    for (int i=0; i<found.top_edges.size();i++) {
        // TO DO if root
        Edge* found_edge = found.top_edges[i];
        Edge* to_replace_edge = to_replace.top_edges[i];
        Node* outside_old_node = found_edge->top;
        Node* inside_old_node = found_edge->bottom;
        to_delete.insert(inside_old_node);

        Node* inside_new_node = to_replace_edge->bottom;
        outside_old_node->bottom.erase(inside_old_node);
        outside_old_node->bottom.insert(inside_new_node);
        inside_new_node->top.insert(outside_old_node);
    }

    // Replace bottom nodes
    for (int i=0; i<found.bottom_edges.size();i++) {
        Edge* found_edge = found.bottom_edges[i];
        Edge* to_replace_edge = to_replace.bottom_edges[i];
        Node* outside_old_node = found_edge->bottom;
        Node* inside_old_node = found_edge->top;
        delete_end_node.insert(inside_old_node);

        Node* inside_new_node = to_replace_edge->top;
        outside_old_node->top.erase(inside_old_node);
        outside_old_node->top.insert(inside_new_node);
        inside_new_node->bottom.insert(outside_old_node);
    }

    // delete old nodes
    std::queue<Node*> to_delete_queue;
    std::set<Node*> visited_nodes;

    for (Node* node : to_delete) {
        visited_nodes.insert(node);
        to_delete_queue.push(node);
    }

    while (!to_delete_queue.empty()) {
        Node* node = to_delete_queue.front();
        to_delete_queue.pop();
        if (!delete_end_node.count(node)) {
            for (Node* nextNode : node->bottom) {
                if (visited_nodes.count(nextNode)) {
                    continue;
                }
                visited_nodes.insert(nextNode);
                to_delete_queue.push(nextNode);
            }
        }
        delete node;
    }
}
