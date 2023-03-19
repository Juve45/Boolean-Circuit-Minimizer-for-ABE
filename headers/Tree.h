#include "abeai.h"
using namespace std;

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
		formula = "";

		for(auto i : edges)
			s.insert(i->formula);

		if(node_type == AND) {
			for(auto i : s)
				formula += i;
		} else if(node_type == INPUT) {

		}
		else {
			formula = "";
			if(parent || s.size() > 1) formula += "(";
			for(auto i : s) {
				formula += i + "+";
			}
			formula.pop_back();
			if(parent || s.size() > 1) formula += ")";
		}
	}

	Tree() { }

	static Tree& from(std::string formula);
};

std::ostream& operator<<(std::ostream& out, const Tree& tree);

vector <vector<Tree*>> our_reduce(Tree * t);
void factorize(Tree * t1, Tree * t2);
void defactorize(Tree * t1, Tree * t2);
