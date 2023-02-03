#include "abeai.h"



SubCircuit pattern1 = *new SubCircuit(CircuitBuilder::from({AND, OR, OR, INPUT, FAN_OUT, INPUT, INPUT}, {
    {0, 1},
    {0, 2},
    {1, 3},
    {1, 4},
    {2, 4},
    {2, 5},
    {4, 6}
}));

SubCircuit to_replace1 = *new SubCircuit(CircuitBuilder::from({AND, OR, INPUT, INPUT, INPUT}, {
    {0, 1},
    {1, 3},
    {0, 2},
    {1, 4}
}));



SubCircuit pattern2 = *new SubCircuit(CircuitBuilder::from({OR, AND, AND, INPUT, FAN_OUT, INPUT, INPUT}, {
    {0, 1},
    {0, 2},
    {1, 3},
    {1, 4},
    {2, 4},
    {2, 5},
    {4, 6}
}));

SubCircuit to_replace2 = *new SubCircuit(CircuitBuilder::from({OR, AND, INPUT, INPUT, INPUT}, {
    {0, 1},
    {1, 3},
    {0, 2},
    {1, 4}
}));




