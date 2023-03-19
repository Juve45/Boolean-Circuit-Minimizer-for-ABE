#include "../headers/abeai.h"
#include "../headers/debug.h"
#include "../headers/Tree.h"

using std::vector;
using std::map;
using std::string;

vector <vector<Tree*>> our_reduce(Tree * t) {
	dbg(t->formula);
	vector <vector<Tree*>> ans;

	if(t->node_type == OR) {

		map<string, vector <Tree *>> similar;

		for(auto i : t->edges)
			for(auto j : i->edges) {
					dbg(i->formula);
					dbg(j->formula);
				if(similar.find(j->formula) != similar.end()) {
					assert(j->parent->parent == similar[j->formula].back()->parent->parent);

					similar[j->formula].push_back(j);
				} else {
					similar[j->formula] = vector <Tree*>();
					similar[j->formula].push_back(j);
				}
			}

		for(auto i : similar)
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
	parent->edges.push_back(child);
	child->parent = parent;
}

void factorize(Tree * t1, Tree * t2) {


	assert(t1->parent);
	assert(t2->parent);
	assert(t1->parent->parent);
	assert(t2->parent->parent);
	assert(t1->parent->parent == t2->parent->parent);

	Tree * and_node = new Tree();
	and_node->node_type = AND;
	Tree * or_node = new Tree();
	or_node->node_type = OR;

	Tree * old1 = t1->parent;
	Tree * old2 = t2->parent;
	dbg_ok;

	auto parent = old1->parent;
	dbg(parent);
	dbg(parent->parent);
	dbg(old1->parent);
	dbg(parent->formula);
	dbg(old1->formula);
	dbg(old2->formula);
	dbg(t1->formula);
	dbg(t2->formula);


	erase_child(parent, old1);
	erase_child(parent, old2);

	add_edge(parent, and_node);
	add_edge(and_node, or_node);
	add_edge(and_node, t1);
	add_edge(or_node, old1);
	add_edge(or_node, old2);
	dbg_ok;
	erase_child(old1, t1);
	erase_child(old2, t2);

	dbg_ok;
	old1->compute_formula();
	old2->compute_formula();

	dbg_ok;
	auto tmp = old2;
	while(tmp) {
		dbg(tmp);
		tmp->compute_formula();
		tmp = tmp->parent;
	}
	dbg(parent);
	dbg(parent->formula);

}

void defactorize(Tree * t1, Tree * t2) {
	dbg(t1->formula);
	dbg(t2->formula);
	assert(t1->parent == t2->parent);
	assert(t1->node_type == OR);
	assert(t2->node_type == OR);

	auto parent = t1->parent;

	Tree * or_node = new Tree();
	or_node->node_type = OR;
	add_edge(parent, or_node);

	for(auto i : t1->edges)
		for(auto j : t2->edges) {
			Tree * tmp = new Tree();
			tmp->node_type = AND;
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



