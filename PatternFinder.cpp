#include <algorithm>
#include "abeai.h"

using std::vector;


bool PatternFinder::isomorph(const vector<Node*> &list_circuit, 
    const vector <Node*> &list_pattern, SubCircuit& pattern) {

    if(list_circuit.size() != list_pattern.size())
        return 0;

    unordered_set<int> checked;
    mapping.clear();

    for(int i = 0; i < list_circuit.size(); i++) {
        if(list_circuit[i]->type != list_pattern[i].type)
            return 0;
        if(list_circuit[i]->top.size() != list_pattern[i].top.size())
            return 0;
        if(list_circuit[i]->bottom.size() != list_pattern[i].bottom.size())
            return 0;

        mapping[list_pattern[i]->id] = list_circuit[i]->id;
    }

    vector <int> inputs, outputs;

    auto check_ngh = [](vector <Node*>& ngh_circ, vector <Node*>& ngh_part) 
    { 
        set <int> ngh_a, ngh_b;
        for(auto j : ngh_circ)
            ngh_a.insert(mapping[j]);

        for(auto j : ngh_part)
            ngh_b.insert(j);

        if(ngh_a != ngh_b)
            return 0;

        return 1;
    };

    for(auto i : pattern.topEdges) {
        
        if(!check_ngh(i->bottom, nodeMapping[mapping[i->id]]->bottom))
            return 0;

        checked.insert(i->id);
        checked.insert(mapping[i->id]);
    }

    for(auto i : pattern.bottomEdges) {

        if(!check_ngh(i->top, nodeMapping[mapping[i->id]]->top))
            return 0;

        if(checked.count(i->id)) 
            assert(false);
        checked.insert(i->id);
        checked.insert(mapping[i->id]);
    }

    for(auto i : list_circuit) {
        if(checked.count(i->id)) 
            continue;
        if(!check_ngh(i->top, nodeMapping[mapping[i->id]]->top))
            return 0;
        if(!check_ngh(i->bottom, nodeMapping[mapping[i->id]]->bottom))
            return 0;
    }

    return 1;
}

void dfs(Node * node, set<int> &used) {
    if(used.count(node->id)) 
        return;

    nodeMapping[node->id] = node;

    used.insert(node->id);

    for(auto i : node->bottom)
        dfs(i);
}


void getNodes(const Circuit &circuit) {

    set <int> used; 
    vector <int> ret;

    dfs(circuit.root, used);
    for(auto i : used)
        ret.push_back(i);
    return ret;
}

void getNodes(const SubCircuit &sub_circuit) {

    set <int> used; 
    vector <int> ret;

    for(auto i : sub_circuit.bottomEdges)
        used.insert(i.st);

    for(auto i : sub_circuit.topEdges)
        dfs(i.nd, used);

    for(auto i : used)
        ret.push_back(i);

    return ret;
}

vector<int> getRandomArrangement(vector<int> source, int k) {
random_shuffle(source.begin(), source.end());
return source.resize(k);
}


SubCircuit createSubFromNodes(vector<int> node_list, const SubCircuit& pattern) {

SubCircuit original;
map<int, std::set<Node*>::iterator> last_edge;

for(auto j : pattern.topEdges) {
    if(last_edge.find(j.bottom->id) == last_edge.end())
        last_edge[j.bottom->id] = j.bottom->top.begin();
    
    original.push_back({last_edge[j.bottom->id]++, 
        nodeMapping[mapping[j.bottom->id]]});
    // original.push_back({NULL, nodeMapping[mapping[j.bottom->id]]});
}

last_edge.clear();

for(auto j : pattern.bottomEdges) {
    if(last_edge.find(j.top->id) == last_edge.end())
        last_edge[j.top->id] = j.top->bottom.begin();
    
    original.push_back({nodeMapping[mapping[j.top->id]], 
        last_edge[j.top->id]++});
    // original.push_back({NULL, nodeMapping[mapping[j.top->id]]});
}

for(auto j : pattern.bottomEdges) 
    original.push_back({nodeMapping[mapping[j.top->id]], NULL});

return original;
}



SubCircuit& findPattern(Circuit& circuit, 
    const SubCircuit& pattern) {

    vector <int> pattern_node_list = getNodes(pattern);
    vector <int> node_list = getNodes(circuit);

    for(int i = 1; i <= 100; i++) {
        vector <int> candidate = 
            getRandomArrangement(pattern_node_list, node_list.size());

        if (isomorph(candidate, pattern))
            return createSubFromNodes(candidate, pattern);
    }
    }

