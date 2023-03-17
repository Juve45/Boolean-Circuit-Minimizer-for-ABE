#include "abeai.h"

class Tree {
public:

	NodeType node_type;
	
	vector <int> formula; 
	// -1 OR (+), 
	// -2 AND (*)

	vector <Tree*> edges;

}
