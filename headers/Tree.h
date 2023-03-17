#include "abeai.h"

class Tree {
public:

	NodeType node_type;
	
	std::string formula; 
	// 0 OR (+),
	// [5, 7, 3, 4, 0, 2, 6, 3, 0] === 5734+263

	std::vector <Tree*> edges;
	Tree * parent;


	void compute_formula() {
		std::set <std::string> s;

		for(auto i : edges)
			s.insert(i->formula);

		if(node_type == AND) {
			for(auto i : s)
				formula += i;
		} else {
			formula = "(";
			for(auto i : s) {
				formula += i + "+";
			}
			formula.pop_back();
			formula += ")";
		}
	}
};
