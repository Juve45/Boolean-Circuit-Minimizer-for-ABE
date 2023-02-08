#ifndef _PATTERNS_H_
#define _PATTERNS_H_

#include "abeai.h"

Subcircuit pattern1 = CircuitBuilder::from({AND, OR, OR, FAN_OUT}, {
    {0, 1},
    {0, 2},
    {1, 3},
    {2, 3}
}, {0}, {1, 3, 2});

Subcircuit replacement1 = CircuitBuilder::from({OR, AND}, {
    {0, 1}
}, {0}, {1, 0, 1});

Subcircuit pattern2 = CircuitBuilder::from({AND, OR, OR, FAN_OUT}, {
    {0, 1},
    {0, 2},
    {1, 3},
    {2, 3}
}, {0}, {1, 3, 2});

Subcircuit replacement2 = CircuitBuilder::from({OR, AND}, {
    {0, 1}
}, {0}, {1, 0, 1});

#endif
