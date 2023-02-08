#include "../headers/abeai.h"
#include "../headers/debug.h"

Circuit& Circuit::copy() {
    std::vector<Node*> new_leaves;
    std::map<Node*, Node*> old_to_new;
    std::function<void(Node*)> dfs = [&](Node* node) {
        old_to_new[node] = new Node(node->type);
        for (Node* lower_node : node->lower)
            if (!old_to_new.count(lower_node))
                dfs(lower_node);
        if (node->lower.empty())
            new_leaves.push_back(old_to_new[node]);
    };
    dfs(root);
    for (const auto& [old_node, new_node] : old_to_new) {
        for (Node* upper_node : old_node->upper)
            new_node->upper.insert(old_to_new[upper_node]);
        for (Node* lower_node : old_node->lower)
            new_node->lower.insert(old_to_new[lower_node]);
    }
    return *(new Circuit(old_to_new[root], new_leaves));
}

std::vector<Node*> Circuit::get_nodes() {
    std::set<Node*> node_set;
    std::function<void(Node*)> dfs = [&](Node* node) {
        node_set.insert(node);
        for (Node* lower_node : node->lower)
            if (!node_set.count(lower_node))
                dfs(lower_node);
    };
    dfs(root);
    std::vector<Node*> nodes;
    std::copy(node_set.begin(), node_set.end(), std::back_inserter(nodes));
    return nodes;
}

int Circuit::eval() {
    std::map<int, int> node_upper_visited = std::map<int, int>();
    std::map<int, int> node_value = std::map<int, int>();

    std::queue<std::pair<Node*, int> > nodes;
    nodes.push({this->root, 1});

    int ans = 0;

    while (!nodes.empty()){
        auto &[node, value] = nodes.front();
        nodes.pop();

        for (Node* next_node : node->lower) {
            node_upper_visited[next_node->id]++;
            node_value[next_node->id] += value;
            if (node_upper_visited[next_node->id] == int(next_node->upper.size())) {
                nodes.push({next_node, node_value[next_node->id]});
            }
        }

        if (node->type == NodeType::INPUT) {
            ans += value;
        }
    }

    return ans;
}

void Circuit::replace_subcircuit(const Subcircuit& found, const Subcircuit& to_replace) {
    assert(found.upper_edges.size() == to_replace.upper_edges.size());
    assert(found.lower_edges.size() == to_replace.lower_edges.size());

    std::set<Node*> to_delete;
    std::set<Node*> delete_end_node;

    // Replace upper nodes
    for (int i=0; i<int(found.upper_edges.size());i++) {
        Edge* found_edge = found.upper_edges[i];
        Edge* to_replace_edge = to_replace.upper_edges[i];
        Node* outside_old_node = found_edge->upper;
        Node* inside_old_node = found_edge->lower;
        to_delete.insert(inside_old_node);

        Node* inside_new_node = to_replace_edge->lower;
        if (outside_old_node) {
            outside_old_node->lower.erase(inside_old_node);
            outside_old_node->lower.insert(inside_new_node);
        } else {
            // inside node is the root
            this->root = inside_new_node;
        }
        inside_new_node->upper.insert(outside_old_node);
    }

    // Replace lower nodes
    for (int i=0; i<int(found.lower_edges.size());i++) {
        Edge* found_edge = found.lower_edges[i];
        Edge* to_replace_edge = to_replace.lower_edges[i];
        Node* outside_old_node = found_edge->lower;
        Node* inside_old_node = found_edge->upper;
        delete_end_node.insert(inside_old_node);

        Node* inside_new_node = to_replace_edge->upper;
        if (outside_old_node) {
            outside_old_node->upper.erase(inside_old_node);
            outside_old_node->upper.insert(inside_new_node);
        }
        inside_new_node->lower.insert(outside_old_node);
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
            for (Node* nextNode : node->lower) {
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
