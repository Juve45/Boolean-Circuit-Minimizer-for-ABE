#include "../headers/abeai.h"
#include "../headers/debug.h"

Circuit& Logic::to_circuit(const std::string& formula) {
    std::vector<Node*> leaves;
    std::vector<std::string> stack;
    std::map<std::string, Node*> mapping;
    for (const char chr : formula) {
        stack.emplace_back(1, chr);
        if ('a' <= chr && chr <= 'z' && !mapping.count(stack.back())) {
            mapping[stack.back()] = new Node(INPUT);
            leaves.push_back(mapping[stack.back()]);
        }
        else if (chr == ')') {
            std::string operation;
            std::vector<std::string> operands;
            while (true) {
                const std::string top = stack.back();
                stack.pop_back();
                if (top == "(") break;
                if (top == "*" || top == "+") operation = top;
                operands.push_back(stack.back());
                stack.pop_back();
            }
            std::string subformula = "(";
            for (int i = operands.size() - 1; i >= 0; i--)
                subformula += operands[i] + operation;
            subformula.back() = ')';
            if (!mapping.count(subformula)) {
                mapping[subformula] = new Node(operation == "*" ? AND : OR);
                Node *upper_node = mapping[subformula];
                for (const auto& operand : operands) {
                    Node *lower_node = mapping[operand];
                    upper_node->lower.insert(lower_node);
                    lower_node->upper.insert(upper_node);
                }
            }
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

std::string Logic::to_formula(const Circuit& circuit) {
    int variable_count = 0;
    std::map<Node*, std::string> mapping;
    std::function<std::string(Node*)> dfs = [&](Node* node) {
        if (!mapping.count(node)) {
            if (node->type == AND || node->type == OR) {
                std::string subformula = "(";
                for (Node* lower_node : node->lower)
                    subformula += dfs(lower_node) + (node->type == AND ? '*' : '+');
                subformula.back() = ')';
                mapping[node] = subformula;
            }
            else if (node->type == FAN_OUT)
                mapping[node] = dfs(*(node->lower.begin()));
            else
                mapping[node] = 'a' + variable_count++;
        }
        return mapping[node];
    };
    return dfs(circuit.root);
}

Subcircuit& Logic::flipped(const Subcircuit& pattern1) {
    Subcircuit *pattern2 = &pattern1.copy();
    auto nodes = pattern2->get_nodes();
    for (Node* node : nodes)
        if (node->type == AND)
            node->type = OR;
        else if (node->type == OR)
            node->type = AND;
    return *pattern2;
}
