#ifndef _PATTERNS_H_
#define _PATTERNS_H_

#include "abeai.h"

std::vector<Subcircuit> patterns;
std::vector<Subcircuit> replacements;

void add_pattern(const Subcircuit& pattern, const Subcircuit& replacement) {
    patterns.push_back(pattern);
    replacements.push_back(replacement);
    patterns.push_back(Utils::conjugate(pattern));
    replacements.push_back(Utils::conjugate(replacement));
}

void load_patterns() {
    add_pattern( // ((a*b)+(a*c)) = (a*(b+c))
        CircuitBuilder::from({AND, OR, OR, FAN_OUT}, {
            {0, 1},
            {0, 2},
            {1, 3},
            {2, 3}
        }, {0}, {1, 3, 2}),
        CircuitBuilder::from({OR, AND}, {
            {0, 1}
        }, {0}, {1, 0, 1})
    );
    add_pattern( // (((a*b)+c)*a) = (a*(b+c))
        CircuitBuilder::from({AND, OR, AND, FAN_OUT}, {
            {0, 1},
            {0, 3},
            {1, 2},
            {2, 3}
        }, {0}, {1, 2, 3}),
        CircuitBuilder::from({AND, OR}, {
            {0, 1}
        }, {0}, {1, 1, 0})
    );
    add_pattern( // (((a*b)+c)*(a+c)) = (c+(a*b))
        CircuitBuilder::from({AND, OR, OR, AND, FAN_OUT, FAN_OUT}, {
            {0, 1},
            {0, 2},
            {1, 4},
            {1, 5},
            {2, 3},
            {2, 5},
            {3, 4}
        }, {0}, {4, 3, 5}),
        CircuitBuilder::from({OR, AND}, {
            {0, 1}
        }, {0}, {1, 1, 0})
    );
    add_pattern( // (((a*b)+(c*d))*(a+c)) = ((a*b)+(c*d))
        CircuitBuilder::from({AND, OR, OR, AND, AND, FAN_OUT, FAN_OUT}, {
            {0, 1},
            {0, 2},
            {1, 5},
            {1, 6},
            {2, 3},
            {2, 4},
            {3, 5},
            {4, 6}
        }, {0}, {5, 3, 6, 4}),
        CircuitBuilder::from({OR, AND, AND}, {
            {0, 1},
            {0, 2}
        }, {0}, {1, 1, 2, 2})
    );
    add_pattern( // (((a*b)+c)*((b*c)+a)) = ((a*c)+(b*(a+c)))
        CircuitBuilder::from({AND, OR, OR, AND, AND, FAN_OUT, FAN_OUT, FAN_OUT}, {
            {0, 1},
            {0, 2},
            {1, 3},
            {1, 7},
            {2, 4},
            {2, 5},
            {3, 5},
            {3, 6},
            {4, 6},
            {4, 7}
        }, {0}, {5, 6, 7}),
        CircuitBuilder::from({OR, AND, OR, AND, FAN_OUT, FAN_OUT}, {
            {0, 1},
            {0, 3},
            {1, 2},
            {2, 4},
            {2, 5},
            {3, 4},
            {3, 5}
        }, {0}, {4, 1, 5})
    );
}

#endif
