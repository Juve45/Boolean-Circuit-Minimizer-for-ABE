#include <algorithm>
#include <unordered_set>
#include <cassert>
#include <chrono>
#include <random>
#include <iostream>
#include "abeai.h"

using std::vector;
using std::set;
using std::map;


bool PatternFinder::isomorph(const vector<int> &list_circuit, 
    const vector <int> &list_pattern, const SubCircuit& pattern) {

    if(list_circuit.size() != list_pattern.size())
        return 0;

    std::unordered_set<int> checked;
    mapping.clear();


    for(int i = 0; i < list_circuit.size(); i++) {
        // if(list_circuit[i]->type != list_pattern[i]->type)
        //     return 0;
        // if(list_circuit[i]->top.size() != list_pattern[i]->top.size())
        //     return 0;
        // if(list_circuit[i]->bottom.size() != list_pattern[i]->bottom.size())
        //     return 0;

        mapping[list_pattern[i]] = list_circuit[i];
    }

    vector <int> inputs, outputs;

    auto check_ngh = [&](set <Node*>& ngh_circ, set <Node*>& ngh_part) 
    { 
        set <int> ngh_a, ngh_b;
        for(auto j : ngh_circ)
            ngh_a.insert(mapping[j->id]);

        for(auto j : ngh_part)
            ngh_b.insert(j->id);

        if(ngh_a != ngh_b)
            return 0;

        return 1;
    };

    for(auto i : pattern.topEdges) {
        
        if(!check_ngh(i->bottom->bottom, Node::fromId[mapping[i->bottom->id]]->bottom))
            return 0;

        checked.insert(i->bottom->id);
        checked.insert(mapping[i->bottom->id]);
    }

    for(auto i : pattern.bottomEdges) {

        if(checked.count(i->top->id)) 
            assert(false);

        if(!check_ngh(i->top->top, Node::fromId[mapping[i->top->id]]->top))
            return 0;

        checked.insert(i->top->id);
        checked.insert(mapping[i->top->id]);
    }

    for(auto i : list_circuit) { // internal nodes, we need to check both up and down
        Node* node = Node::fromId[i];
        if(checked.count(i)) 
            continue;
        if(!check_ngh(node->top, Node::fromId[mapping[i]]->top))
            return 0;
        if(!check_ngh(node->bottom, Node::fromId[mapping[i]]->bottom))
            return 0;
    }

    return 1;
}

void dfs(Node * node, set<int> &used) {
    if(used.count(node->id)) 
        return;

    used.insert(node->id);

    for(auto i : node->bottom)
        dfs(i, used);
}


vector<int> getNodes(const Circuit &circuit) {

    set <int> used; 
    vector <int> ret;

    dfs(circuit.root, used);
    for(auto i : used)
        ret.push_back(i);
    return ret;
}

vector<int> getNodes(const SubCircuit &sub_circuit) {

    std::cerr << "ok" << std::endl;
    set <int> used; 
    vector <int> ret;

    for(auto i : sub_circuit.bottomEdges) {

        std::cerr << "ok = " << i << std::endl;
        used.insert(i->top->id);
    }

    std::cerr << "ok" << std::endl;

    for(auto i : sub_circuit.topEdges)
        dfs(i->bottom, used);

    for(auto i : used)
        ret.push_back(i);

    return ret;
}

vector<int> getRandomArrangement(vector<int> source, int k) {

    std::mt19937 rand(std::chrono::steady_clock::now().time_since_epoch().count());
    shuffle(source.begin(), source.end(), rand);
    source.resize(k);
    return source;
}


SubCircuit PatternFinder::createSubFromNodes(vector<int> node_list, const SubCircuit& pattern) {

    SubCircuit original;
    map<int, std::set<Node*>::iterator> last_edge;

    for(auto j : pattern.topEdges) {
        if(last_edge.find(j->bottom->id) == last_edge.end())
            last_edge[j->bottom->id] = j->bottom->top.begin();
        
        original.topEdges.push_back(
            new Edge(*(last_edge[j->bottom->id]++), 
            Node::fromId[mapping[j->bottom->id]]));
        // original.push_back({NULL, nodeMapping[mapping[j.bottom->id]]});
    }

    last_edge.clear();

    for(auto j : pattern.bottomEdges) {
        if(last_edge.find(j->top->id) == last_edge.end())
            last_edge[j->top->id] = j->top->bottom.begin();
        
        original.topEdges.push_back(new Edge(
            Node::fromId[mapping[j->top->id]], 
            *(last_edge[j->top->id]++)));
        // original.push_back({NULL, nodeMapping[mapping[j.top->id]]});
    }

    return original;
}



SubCircuit PatternFinder::findPattern(Circuit& circuit, 
    const SubCircuit& pattern) {


    std::cerr << "ok1" << std::endl;

    vector <int> pattern_node_list = getNodes(pattern);

    std::cerr << "ok" << std::endl;
    vector <int> node_list = getNodes(circuit);

    for(int i = 1; i <= 100; i++) {
        vector <int> candidate = 
            getRandomArrangement(pattern_node_list, node_list.size());

        if (isomorph(candidate, pattern_node_list, pattern))
            return createSubFromNodes(candidate, pattern);
    }

    return SubCircuit();
}

