#include <functional>
#include "abeai.h"

SubCircuit* PatternFinder::find_pattern(Circuit& circuit, SubCircuit& pattern) {
    auto circuit_nodes = circuit.get_nodes();
    auto pattern_nodes = pattern.get_nodes();

    std::set<Node*> pattern_root_nodes;
    for (Edge* edge : pattern.top_edges)
        pattern_root_nodes.insert(edge->bottom);
    std::set<Node*> pattern_leaf_nodes;
    for (Edge* edge : pattern.bottom_edges)
        pattern_leaf_nodes.insert(edge->top);

    std::map<Node*, int> leaf_degrees;
    for (Edge* edge : pattern.bottom_edges)
        leaf_degrees[edge->top]++;

    SubCircuit *match = nullptr;
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
                if (!pattern_root_nodes.count(node1) && !same(node1->top, node2->top)) {
                    isomorph = false;
                    break;
                }
                if (!same(node1->bottom, node2->bottom)) {
                    isomorph = false;
                    break;
                }
            }
            if (!isomorph) return false;

            std::map<Node*, std::vector<Node*>> leaf_edges;
            for (Node* leaf : pattern_leaf_nodes) {
                Node *match_leaf = mapping[leaf];
                for (Node* bottom_node : match_leaf->bottom)
                    if (!used.count(bottom_node))
                        leaf_edges[match_leaf].push_back(bottom_node);
            }
            for (Node* leaf : pattern_leaf_nodes)
                if (leaf_degrees[leaf] != (int)leaf_edges[mapping[leaf]].size()) return false;

            match = new SubCircuit();
            for (Edge* edge : pattern.top_edges) {
                Node *root = mapping[edge->bottom];
                match->top_edges.push_back(new Edge(root->top.empty() ? nullptr : *(root->top.begin()), root));
            }
            for (Edge* edge : pattern.bottom_edges) {
                Node *leaf = mapping[edge->top];
                match->bottom_edges.push_back(new Edge(leaf, leaf_edges[leaf].back()));
                leaf_edges[leaf].pop_back();
            }
            return true;
        }
        for (Node* node : circuit_nodes) {
            if (used.count(node)) continue;
            if (node->type != pattern_nodes[pos]->type) continue;
            const bool is_root = pattern_root_nodes.count(pattern_nodes[pos]);
            const bool is_leaf = pattern_leaf_nodes.count(pattern_nodes[pos]);
            const bool same_top = node->top.size() == pattern_nodes[pos]->top.size();
            const bool same_bottom = node->bottom.size() == pattern_nodes[pos]->bottom.size();
            if (is_root && !same_bottom) continue;
            if (is_leaf && !(same_top && leaf_degrees[pattern_nodes[pos]] + pattern_nodes[pos]->bottom.size() == node->bottom.size())) continue;
            if (!is_root && !is_leaf && !(same_top && same_bottom)) continue;
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
