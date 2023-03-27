#include "../headers/abeai.h"
#include "../headers/debug.h"

int Node::node_count;
std::map<int, Node*> Node::from_id;

std::vector<Node*> Circuit::get_nodes() const {
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

Circuit& Circuit::copy() const {
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
    return *new Circuit(old_to_new[root], new_leaves);
}

int Circuit::eval() const {
    std::map<int, int> node_upper_visited;
    std::map<int, int> node_value;

    std::queue<std::pair<Node*, int>> nodes;
    nodes.push({this->root, 1});

    int ans = 0;
    while (!nodes.empty()) {
        auto& [node, value] = nodes.front();
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

void Circuit::check() const {
    int leaf_count = 0;
    for (Node* node : get_nodes()) {
        leaf_count += node->lower.empty();
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
    if (leaf_count != int(leaves.size()))
        std::cerr << "ERROR: more leafs than expected\n";
}

Subcircuit* Circuit::find_pattern(const Subcircuit& pattern) const {
    auto circuit_nodes = get_nodes();
    auto pattern_nodes = pattern.get_nodes();

    std::set<Node*> pattern_root_nodes;
    for (Edge* edge : pattern.upper_edges)
        pattern_root_nodes.insert(edge->lower);
    std::set<Node*> pattern_leaf_nodes;
    for (Edge* edge : pattern.lower_edges)
        pattern_leaf_nodes.insert(edge->upper);

    std::map<Node*, int> leaf_degrees;
    for (Edge* edge : pattern.lower_edges)
        leaf_degrees[edge->upper]++;

    Subcircuit *match = nullptr;
    std::vector<Node*> match_nodes(pattern_nodes.size());

    std::set<Node*> used;
    std::function<bool(int)> bkt = [&](int pos) {
        if (pos == int(pattern_nodes.size())) {
            std::map<Node*, Node*> mapping;
            for (int i = 0; i < int(pattern_nodes.size()); i++)
                mapping[pattern_nodes[i]] = match_nodes[i];

            auto same = [&](std::set<Node*> set1, std::set<Node*> set2) {
                for (Node* node1 : set1) {
                    Node *node2 = mapping[node1];
                    if (!set2.count(node2)) return false;
                }
                return true;
            };

            bool isomorph = true;
            for (const auto& [node1, node2] : mapping) {
                if (!pattern_root_nodes.count(node1) && !same(node1->upper, node2->upper)) {
                    isomorph = false;
                    break;
                }
                if (!same(node1->lower, node2->lower)) {
                    isomorph = false;
                    break;
                }
            }
            if (!isomorph) return false;

            std::map<Node*, std::vector<Node*>> leaf_edges;
            for (Node* leaf : pattern_leaf_nodes) {
                Node *match_leaf = mapping[leaf];
                for (Node* lower_node : match_leaf->lower)
                    if (!used.count(lower_node))
                        leaf_edges[match_leaf].push_back(lower_node);
            }
            for (Node* leaf : pattern_leaf_nodes)
                if (leaf_degrees[leaf] != (int)leaf_edges[mapping[leaf]].size()) return false;

            std::vector<Edge*> upper_edges;
            for (Edge* edge : pattern.upper_edges) {
                Node *root = mapping[edge->lower];
                upper_edges.push_back(new Edge(root->upper.empty() ? nullptr : *(root->upper.begin()), root));
            }
            std::vector<Edge*> lower_edges;
            for (Edge* edge : pattern.lower_edges) {
                Node *leaf = mapping[edge->upper];
                lower_edges.push_back(new Edge(leaf, leaf_edges[leaf].back()));
                leaf_edges[leaf].pop_back();
            }
            match = new Subcircuit(upper_edges, lower_edges);
            return true;
        }
        for (Node* node : circuit_nodes) {
            if (used.count(node)) continue;
            if (node->type != pattern_nodes[pos]->type) continue;
            const bool is_root = pattern_root_nodes.count(pattern_nodes[pos]);
            const bool is_leaf = pattern_leaf_nodes.count(pattern_nodes[pos]);
            const bool same_upper = node->upper.size() == pattern_nodes[pos]->upper.size();
            const bool same_lower = node->lower.size() == pattern_nodes[pos]->lower.size();
            if (is_root && !same_lower) continue;
            if (is_leaf && !(same_upper && leaf_degrees[pattern_nodes[pos]] + pattern_nodes[pos]->lower.size() == node->lower.size())) continue;
            if (!is_root && !is_leaf && !(same_upper && same_lower)) continue;
            match_nodes[pos] = node;
            used.insert(node);
            if (bkt(pos + 1)) return true;
            used.erase(node);
        }
        return false;
    };
    bkt(0);
    return match;
}

void Circuit::replace_subcircuit(const Subcircuit& found, const Subcircuit& replacement) {
    if (replacement.upper_edges.empty()) {
        Node *upper = replacement.upper_edges[0]->upper;
        Node *lower = replacement.lower_edges[0]->lower;
        upper->lower.erase(replacement.upper_edges[0]->lower);
        lower->upper.erase(replacement.lower_edges[0]->upper);
        for (Edge* edge : replacement.lower_edges)
            edge->lower = nullptr;
        auto nodes = found.get_nodes();
        for (Node* node : nodes)
            delete node;
        upper->lower.insert(lower);
        lower->upper.insert(upper);
        return;
    }

    assert(found.upper_edges.size() == replacement.upper_edges.size());
    assert(found.lower_edges.size() == replacement.lower_edges.size());

    std::set<Node*> to_delete;
    std::set<Node*> delete_end_node;

    // replace upper nodes
    for (int i = 0; i < int(found.upper_edges.size()); i++) {
        Edge *found_edge = found.upper_edges[i];
        Edge *replacement_edge = replacement.upper_edges[i];
        Node *outside_old_node = found_edge->upper;
        Node *inside_old_node = found_edge->lower;
        to_delete.insert(inside_old_node);

        Node *inside_new_node = replacement_edge->lower;
        if (outside_old_node) {
            outside_old_node->lower.erase(inside_old_node);
            outside_old_node->lower.insert(inside_new_node);
        }
        else // inside node is the root
            this->root = inside_new_node;
        inside_new_node->upper.insert(outside_old_node);
    }

    // replace lower nodes
    for (int i = 0; i < int(found.lower_edges.size()); i++) {
        Edge *found_edge = found.lower_edges[i];
        Edge *replacement_edge = replacement.lower_edges[i];
        Node *outside_old_node = found_edge->lower;
        Node *inside_old_node = found_edge->upper;
        delete_end_node.insert(inside_old_node);

        Node *inside_new_node = replacement_edge->upper;
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
        Node *node = to_delete_queue.front();
        to_delete_queue.pop();
        if (!delete_end_node.count(node)) {
            for (Node* next_node : node->lower) {
                if (visited_nodes.count(next_node)) continue;
                visited_nodes.insert(next_node);
                to_delete_queue.push(next_node);
            }
        }
        delete node;
    }
}
