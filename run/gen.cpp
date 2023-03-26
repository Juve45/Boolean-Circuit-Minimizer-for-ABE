#include "../headers/abeai.h"
#include "../headers/debug.h"
#include "../headers/patterns.h"
#include "../headers/Tree.h"

int get_random(int min, int max) {
	static std::mt19937 rand(std::chrono::steady_clock::now().time_since_epoch().count());
	return min + rand() % (max - min + 1);
}

int main() {
    ofstream fout("formulas.txt");
    for (int i = 0; i < 50; i++) {
        const Circuit circuit = CircuitBuilder::random(get_random(5, 20), get_random(2, 5));
        const string formula = Utils::to_formula(circuit);
        fout << formula << '\n';
    }
    return 0;
}
