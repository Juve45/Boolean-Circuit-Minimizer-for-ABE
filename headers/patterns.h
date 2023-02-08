#ifndef _PATTERNS_H_
#define _PATTERNS_H_

#include "abeai.h"

Subcircuit *pattern1 = new Subcircuit({AND, OR, OR, FAN_OUT}, {
    {-1, 0},
    {0, 1},
    {0, 2},
    {1, -1},
    {1, 3},
    {2, 3},
    {2, -1},
    {3, -1}
});

Subcircuit *to_replace1 = new Subcircuit({AND, OR}, {
    {-1, 0},
    {0, 1},
    {1, -1},
    {0, 2},
    {1, -1}
});

Subcircuit *pattern2 = new Subcircuit({OR, AND, AND, FAN_OUT}, {
    {-1, 0},
    {0, 1},
    {0, 2},
    {1, -1},
    {1, 3},
    {2, 3},
    {2, -1},
    {3, -1}
});

Subcircuit *to_replace2 = new Subcircuit({OR, AND}, {
    {-1, 0},
    {0, 1},
    {1, -1},
    {0, 2},
    {1, -1}
});

#endif
