#ifndef _PATTERNS_H_
#define _PATTERNS_H_
#include "abeai.h"



std::vector <std::pair<SubCircuit*, SubCircuit*>> gen_patterns() {

    std::vector <std::pair<SubCircuit*, SubCircuit*>> ret;

    SubCircuit *pattern1 = new SubCircuit({AND, OR, OR, FAN_OUT}, {
        {-1, 0},
        {0, 1},
        {0, 2},
        {1, -1},
        {1, 3},
        {2, 3},
        {2, -1},
        {3, -1}
    });

    SubCircuit *to_replace1 = new SubCircuit({AND, OR}, {
        {-1, 0},
        {0, 1},
        {1, -1},
        {0, 2},
        {1, -1}
    });

    ret.push_back({pattern1, to_replace1});

    SubCircuit *pattern2 = new SubCircuit({OR, AND, AND, FAN_OUT}, {
        {-1, 0},
        {0, 1},
        {0, 2},
        {1, -1},
        {1, 3},
        {2, 3},
        {2, -1},
        {3, -1}
    });

    SubCircuit *to_replace2 = new SubCircuit({OR, AND}, {
        {-1, 0},
        {0, 1},
        {1, -1},
        {0, 2},
        {1, -1}
    });

    ret.push_back({pattern2, to_replace2});


    return ret;
}

#endif