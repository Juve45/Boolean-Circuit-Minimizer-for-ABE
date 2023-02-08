#ifndef _PATTERNS_H_
#define _PATTERNS_H_

#include "abeai.h"

Subcircuit pattern1({AND, OR, OR, FAN_OUT}, {
    {-1, 0},
    {0, 1},
    {0, 2},
    {1, -1},
    {1, 3},
    {2, 3},
    {2, -1},
    {3, -1}
});

Subcircuit replacement1({AND, OR}, {
    {-1, 0},
    {0, 1},
    {1, -1},
    {0, 2},
    {1, -1}
});

Subcircuit pattern2({OR, AND, AND, FAN_OUT}, {
    {-1, 0},
    {0, 1},
    {0, 2},
    {1, -1},
    {1, 3},
    {2, 3},
    {2, -1},
    {3, -1}
});

Subcircuit replacement2({OR, AND}, {
    {-1, 0},
    {0, 1},
    {1, -1},
    {0, 2},
    {1, -1}
});

#endif
