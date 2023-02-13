#include "../headers/abeai.h"
#include "../headers/debug.h"

int Node::node_count;
std::map<int, Node*> Node::from_id;

void Utils::check_circuit(const Circuit& circuit) {
    int leaf_count = 0;
    for (Node* node : circuit.get_nodes()) {
        leaf_count += node->lower.empty();
        if (node->type == INPUT && node->upper.size() != 1)
            std::cerr << "ERROR: input.upper_count = " << node->upper.size() << '\n';
        else if (node->type == INPUT && node->lower.size() != 0)
            std::cerr << "ERROR: input.lower_count = " << node->lower.size() << '\n';
        else if (node->type == FAN_OUT && node->upper.size() < 2)
            std::cerr << "ERROR: fan_out.upper_count = " << node->upper.size() << '\n';
        else if (node->type == FAN_OUT && node->lower.size() != 1)
            std::cerr << "ERROR: fan_out.lower_count = " << node->lower.size() << '\n';
        else if ((node->type == AND || node->type == OR) && !((node == circuit.root && node->upper.size() == 0) || (node != circuit.root && node->upper.size() == 1)))
            std::cerr << "ERROR: and_or.upper_count = " << node->upper.size() << '\n';
        else if ((node->type == AND || node->type == OR) && node->lower.size() < 2)
            std::cerr << "ERROR: and_or.lower_count = " << node->lower.size() << '\n';
    }
    if (leaf_count != int(circuit.leaves.size()))
        std::cerr << "ERROR: more leafs than expected\n";
}

std::vector<int> Utils::random_partition(int value, int parts) {
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

Circuit& Utils::to_circuit(const std::string& formula) {
    std::vector<Node*> leaves;
    std::vector<std::string> stack;
    std::map<std::string, Node*> mapping;
    for (const char chr : formula) {
        if (chr == '*' || chr == '+' || ('a' <= chr && chr <= 'z'))
            stack.emplace_back(1, chr);
        if ('a' <= chr && chr <= 'z' && !mapping.count(stack.back())) {
            mapping[stack.back()] = new Node(INPUT);
            leaves.push_back(mapping[stack.back()]);
        }
        else if (chr == ')') {
            const std::string lhs = stack[stack.size() - 3];
            const std::string opr = stack[stack.size() - 2];
            const std::string rhs = stack[stack.size() - 1];
            std::string subformula = "(" + lhs + opr + rhs + ")";
            if (!mapping.count(subformula)) {
                mapping[subformula] = new Node(opr == "*" ? AND : OR);
                Node *lhs_node = mapping[lhs];
                Node *rhs_node = mapping[rhs];
                Node *big_node = mapping[subformula];
                big_node->lower.insert(lhs_node); lhs_node->upper.insert(big_node);
                big_node->lower.insert(rhs_node); rhs_node->upper.insert(big_node);
            }
            stack.pop_back();
            stack.pop_back();
            stack.pop_back();
            stack.push_back(subformula);
        }
    }
    Circuit *circuit = new Circuit(mapping[stack.back()], leaves);
    auto nodes = circuit->get_nodes();
    for (Node* node : nodes)
        if (node->upper.size() > 1) {
            Node *fan_out_node = new Node(FAN_OUT);
            for (Node* upper_node : node->upper) {
                fan_out_node->upper.insert(upper_node);
                upper_node->lower.erase(node);
                upper_node->lower.insert(fan_out_node);
            }
            node->upper.clear();
            node->upper.insert(fan_out_node);
            fan_out_node->lower.insert(node);
        }
    return *circuit;
}

std::string Utils::to_formula(const Circuit& circuit) {
    int variable_count = 0;
    std::map<Node*, std::string> mapping;
    std::function<std::string(Node*)> dfs = [&](Node* node) {
        if (!mapping.count(node)) {
            if (node->type == AND || node->type == OR)
                mapping[node]
                    = "("
                    + dfs(*(node->lower.begin()))
                    + (node->type == AND ? '*' : '+')
                    + dfs(*(node->lower.rbegin()))
                    + ")";
            else if (node->type == FAN_OUT)
                mapping[node] = dfs(*(node->lower.begin()));
            else
                mapping[node] = 'a' + variable_count++;
        }
        return mapping[node];
    };
    return dfs(circuit.root);
}
