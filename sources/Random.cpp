#include "../headers/abeai.h"
#include "../headers/debug.h"

int Random::integer(int max) {
    static std::mt19937 rand(std::chrono::steady_clock::now().time_since_epoch().count());
    return rand() % max;
}

int Random::integer(int min, int max) {
    return min + integer(max - min);
}

std::pair<int, int> Random::two_integers(int max) {
    assert(max > 1);
    int a = integer(max);
    int b = integer(max);
    while (a == b)
        b = integer(max);
    return std::make_pair(a, b);
}

std::vector<int> Random::partition(int value, int parts) {
    std::mt19937 rand(std::chrono::steady_clock::now().time_since_epoch().count());
    std::vector<int> partition(parts);
    for (int i = 0; i < parts - 1; i++) {
        partition[i] = rand() % (value - parts + i + 1) + 1;
        value -= partition[i];
    }
    partition[parts - 1] = value;
    std::shuffle(partition.begin(), partition.end(), rand);
    return partition;
}
