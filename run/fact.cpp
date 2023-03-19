#include "../headers/Tree.h"
#include "../headers/debug.h"
using namespace std;

// return a random number from [0, mx)
int get_random(int mx) {
	static std::mt19937 rand(std::chrono::steady_clock::now().time_since_epoch().count());
	return rand() % mx;
}
 
// return two distinct random numbers from [0,mx)
pair<int, int> get_two_randoms(int mx) {
	assert(mx > 1);
    int a = get_random(mx);
	int b = get_random(mx);
	while (b == a){
		b = get_random(mx);
	}
	return {a,b};
}
 
void hill_climbing(Tree *t) {
	int i = 0;
	while (true) {
		vector<vector<Tree*>> factorizable = our_reduce(t);
		if (factorizable.empty()) {
			// we can't optimize further
			break;
		}
		int c = get_random(factorizable.size());
		assert(factorizable[c].size() > 1);
 
		auto [f1, f2] = get_two_randoms(factorizable[c].size());
		factorize(factorizable[c][f1], factorizable[c][f2]);
	}
}
 
Tree* get_random_and(Tree *root) {
	vector<Tree*> and_nodes;
	queue<Tree *> nodes;
	nodes.push(root);
	while (!nodes.empty()) {
		Tree *nod = nodes.front();
		if (nod->node_type == AND) {
			and_nodes.push_back(nod);
		}
		nodes.pop();
		for (auto i : nod->edges) {
			nodes.push(i);
		}
	}
	int i = get_random(and_nodes.size());
	return and_nodes[i];
}
 
void simulated_annealing(Tree *root, int k_max = 100) {
	for (int k=0;k<k_max;k++){
		if (get_random(2*k_max) < k_max - k){
			// defactorize
			Tree* and_node = get_random_and(root);
			vector<Tree*> or_nodes;
			for (auto i : and_node->edges) {
				if (i->node_type == OR) {
					or_nodes.push_back(i);
				}
			}
			if (or_nodes.size() >= 2) {
				auto [f1, f2] = get_two_randoms(or_nodes.size());
				defactorize(or_nodes[f1], or_nodes[f2]);
			}
		} else {
			// factorize
			vector<vector<Tree*>> factorizable = our_reduce(root);
			if (factorizable.empty()) {
				// we can't optimize further
				continue;
			}
			int c = get_random(factorizable.size());
			assert(factorizable[c].size() > 1);
 
			auto [f1, f2] = get_two_randoms(factorizable[c].size());
			factorize(factorizable[c][f1], factorizable[c][f2]);
		}
	}
}

int main() {

	Tree * tree = &Tree::from("((a*b*c)+(a*d)+(b*c*d))");
	hill_climbing(tree);
	cout << tree->formula << '\n';
}