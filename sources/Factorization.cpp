#include "../headers/abeai.h"
#include "../headers/debug.h"
#include "../headers/Tree.h"

using std::vector;
using std::map;
using std::string;

vector <vector<Tree*>> our_reduce(Tree * t) {
	vector <vector<Tree*>> ans;

	if(t->node_type == OR) {

		map<string, vector <Tree *>> similar;

		for(auto i : t->edges)
			for(auto j : i->edges) {
				if(similar.find(j->formula) != similar.end()) {
					assert(j->parent->parent == similar[j->formula].back()->parent->parent);
					if(similar[j->formula].back()->parent == j->parent) {
						throw std::runtime_error("Two siblings with same parent! Please implement absorbtion");
					}
				}
				else 
					similar[j->formula] = vector <Tree*>();
				similar[j->formula].push_back(j);
			}

		for(auto i : similar) 
			if(i.second.size() > 1)
				ans.push_back(i.second);
		
	} 

	for(auto i : t->edges) {
		auto ans_child = our_reduce(i);
		for(auto i : ans_child)
			ans.push_back(i);
	}
	return ans;
}

void erase_child(Tree * parent, Tree * child) {
	auto it = find(parent->edges.begin(), parent->edges.end(), child);
	if(it != parent->edges.end())
		parent->edges.erase(it);
	else assert(false);
}

void add_edge(Tree * parent, Tree * child) {
	assert(parent);
	assert(child);
	parent->edges.push_back(child);
	child->parent = parent;
}


void trimm(Tree * t) {
	if(t->parent && t->node_type != INPUT) {
		if(t->edges.size() == 0) 
			erase_child(t->parent, t);
		if(t->edges.size() == 1) {
			Tree * child = t->edges[0];
			if(child->node_type != INPUT) {
				assert(t->parent->node_type == child->node_type);
				for(auto i : child->edges)
					add_edge(t->parent, i);
			} else 
				add_edge(t->parent, child);
			
			erase_child(t->parent, t);
		}
	}
	for(auto i : t->edges)
		trimm(i);
}

void dfs(Tree * t) {
	
	if(t->node_type == AND) cout << "AND ";
	if(t->node_type == OR) cout << "OR ";
	if(t->node_type == INPUT) cout << "INPUT ";
	cout << t->formula << ' ' << t->edges.size() << endl;

	for(auto i : t->edges)
		dfs(i);
}

void factorize(Tree * t1, Tree * t2) {


	assert(t1->parent);
	assert(t2->parent);
	assert(t1->parent->parent);
	assert(t2->parent->parent);
	assert(t1->parent->parent == t2->parent->parent);

	Tree * and_node = new Tree(AND);
	Tree * or_node = new Tree(OR);

	Tree * old1 = t1->parent;
	Tree * old2 = t2->parent;

	auto parent = old1->parent;

	erase_child(parent, old1);
	erase_child(parent, old2);

	add_edge(or_node, old1);
	add_edge(or_node, old2);
	add_edge(and_node, or_node);
	add_edge(and_node, t1);
	add_edge(parent, and_node);

	erase_child(old1, t1);
	erase_child(old2, t2);

	old1->compute_formula();
	old2->compute_formula();

	auto tmp = old2;
	while(tmp) {
		tmp->compute_formula();
		tmp = tmp->parent;
	}
	// dfs(parent);
	trimm(parent);
	// dfs(parent);
}

void defactorize(Tree * t1, Tree * t2) {
	assert(t1->parent == t2->parent);
	assert(t1->node_type == OR);
	assert(t2->node_type == OR);

	auto parent = t1->parent;

	Tree * or_node = new Tree(OR);
	add_edge(parent, or_node);

	for(auto i : t1->edges)
		for(auto j : t2->edges) {
			Tree * tmp = new Tree(AND);
			for(auto k : i->edges) 
				add_edge(tmp, k);
			
			for(auto k : j->edges)
				add_edge(tmp, k);

			tmp->compute_formula();
			add_edge(or_node, tmp);
		}

	erase_child(parent, t1);
	erase_child(parent, t2);

	auto tmp = or_node;
	while(tmp) {
		tmp->compute_formula();
		tmp = tmp->parent;
	}
}



