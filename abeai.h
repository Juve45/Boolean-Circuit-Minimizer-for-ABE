#include <vector>
#include <set>


enum NodeType {
	AND, 
	OR, 
	FO
};


struct Node {
	static int node_count;
	int id;
	NodeType type;
	std::set <Node*> up, down;

	Node() {
		id = node_count++;
	}
};


struct Edge {
	static int edge_count;
	int id;
	Node* up, * down;

	Edge() {
		id = edge_count++;
	}
};

class SubCircuit {
	std::vector<Edge*> up_edges, down_edges;
};

class Circuit {

	Node* root;
	std::vector <Node*> leafs;

	int eval(); // using cost function (no of paths)

	void swapSubCircuit(const SubCircuit& current,
						const SubCircuit& new_circuit);
};

class CircuitBuilder {

	int height; // length of max path from root to some leaf
	int leafs; // no of leafs
	int node_count; // number of internal nodes


	Circuit& build();

};


class PatternFinder {

	SubCircuit& find_pattern(Circuit& circuit, const SubCircuit& pattern_circuit);

};