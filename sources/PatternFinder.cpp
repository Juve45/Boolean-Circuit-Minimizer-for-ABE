#include "../headers/abeai.h"
#include "../headers/debug.h"

Subcircuit* PatternFinder::find_pattern(const Circuit& circuit, const Subcircuit& pattern) {
    auto circuit_nodes = circuit.get_nodes();
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
