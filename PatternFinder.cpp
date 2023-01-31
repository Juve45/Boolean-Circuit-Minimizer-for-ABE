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

    std::vector<Node*> new_nodes(pattern_nodes.size());
    std::set<Node*> used;
    SubCircuit *match = nullptr;

    std::function<bool(int)> bkt = [&](int pos) {
        if (pos == int(pattern_nodes.size())) {
            std::map<Node*, Node*> mapping;
            for (int i = 0; i < int(pattern_nodes.size()); i++)
                mapping[pattern_nodes[i]] = new_nodes[i];

            auto same_set = [&](std::set<Node*> set1, std::set<Node*> set2) {
                for (Node* node1 : set1) {
                    Node *node2 = mapping[node1];
                    if (!set2.count(node2)) return false;
                }
                return true;
            };

            bool isomorph = true;
            for (const auto& [node1, node2] : mapping) {
                if (!node1->top.empty() && !same_set(node1->top, node2->top)) {
                    isomorph = false;
                    break;
                }
                if (!node1->bottom.empty() && !same_set(node1->bottom, node2->bottom)) {
                    isomorph = false;
                    break;
                }
            }
            if (isomorph) {
                match = new SubCircuit();
                for (Edge* edge : pattern.top_edges) {
                    if (mapping[edge->bottom]->top.empty())
                        match->top_edges.push_back(new Edge(nullptr, mapping[edge->bottom]));
                    for (Node* top_node : mapping[edge->bottom]->top)
                        match->top_edges.push_back(new Edge(top_node, mapping[edge->bottom]));
                }
                for (Edge* edge : pattern.bottom_edges) {
                    if (mapping[edge->top]->bottom.empty())
                        match->bottom_edges.push_back(new Edge(mapping[edge->top], nullptr));
                    for (Node* bottom_node : mapping[edge->top]->bottom)
                        match->bottom_edges.push_back(new Edge(mapping[edge->top], bottom_node));
                }
            }
            return match != nullptr;
        }
        for (Node* node : circuit_nodes) {
            if (used.count(node)) continue;
            if (pattern_nodes[pos]->type != INPUT && node->type != pattern_nodes[pos]->type) continue;
            const bool is_root = pattern_root_nodes.count(pattern_nodes[pos]);
            const bool is_leaf = pattern_leaf_nodes.count(pattern_nodes[pos]);
            const bool same_top = node->top.size() == pattern_nodes[pos]->top.size();
            const bool same_bottom = node->bottom.size() == pattern_nodes[pos]->bottom.size();
            if (is_root && !same_bottom) continue;
            if (is_leaf && !same_top) continue;
            if (!is_root && !is_leaf && (!same_top || !same_bottom)) continue;
            new_nodes[pos] = node;
            used.insert(node);
            if (bkt(pos + 1)) return true;
            used.erase(node);
        }
        return false;
    };
    bkt(0);
    return match;
}
