#include "../headers/abeai.h"
#include "../headers/debug.h"

Circuit& Logic::to_circuit(const std::string& formula) {
    std::vector<Node*> leaves;
    std::vector<std::string> stack;
    std::map<std::string, Node*> mapping;
    for (const char chr : formula) {
        stack.emplace_back(1, chr);
        if (isalpha(chr) && !mapping.count(stack.back())) {
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

Tree& Logic::to_tree(const std::string& formula) {
    std::string new_formula = "(";
    for (const char chr : formula) {
        if (isalnum(new_formula.back()) && isalpha(chr))
            new_formula += '*';
        new_formula += chr;
    }
    new_formula += ')';

    std::stack<char> operatorStack;
    std::stack<Tree*> operandStack;
    for (int i = 0; i < int(new_formula.size()); i++) {
        const char chr = new_formula[i];
        if (isalpha(chr)) {
            Tree *node = new Tree(INPUT);
            node->formula = std::string(1, chr);
            operandStack.push(node);
        }
        else if (isdigit(chr))
            operandStack.top()->formula += chr;
        else {
            operatorStack.push(chr);
            if (chr == ')') {
                std::vector<char> operators;
                std::vector<Tree*> operands;
                while (true) {
                    const char topOperator = operatorStack.top();
                    operatorStack.pop();
                    if (topOperator == '(') {
                        std::reverse(operators.begin(), operators.end());
                        std::reverse(operands.begin(), operands.end());
                        std::vector<Tree*> terms;
                        std::vector<Tree*> factors(1, operands.front());
                        for (int i = 1; i < int(operands.size()); i++)
                            if (operators[i - 1] == '*')
                                factors.push_back(operands[i]);
                            else {
                                Tree *product = new Tree(AND);
                                for (Tree* factor : factors)
                                    product->children.push_back(factor);
                                terms.push_back(product);
                                factors.clear();
                                factors.push_back(operands[i]);
                            }
                        Tree *product = new Tree(AND);
                        for (Tree* factor : factors)
                            product->children.push_back(factor);
                        terms.push_back(product);
                        Tree *sum = new Tree(OR);
                        for (Tree* term : terms)
                            sum->children.push_back(term);
                        operandStack.push(sum);
                        break;
                    }
                    Tree *topOperand = operandStack.top();
                    operandStack.pop();
                    if (topOperator != ')') operators.push_back(topOperator);
                    operands.push_back(topOperand);
                }
            }
        }
    }

    Tree *tree = operandStack.top();
    std::function<void(Tree*)> clean = [&](Tree* tree) {
        for (Tree* child : tree->children)
            clean(child);
        if (tree->children.size() == 1) {
            Tree *child = tree->children.back();
            tree->children.pop_back();
            for (Tree* grand : child->children)
                tree->children.push_back(grand);
            tree->type = child->type;
            tree->formula = child->formula;
            delete child;
        }
    };
    clean(tree);

    std::function<void(Tree*)> make_formula = [&](Tree* tree) {
        for (Tree* child : tree->children) {
            make_formula(child);
            child->parent = tree;
        }
        if (tree->children.empty()) return;
        tree->formula = "(";
        for (Tree* child : tree->children)
            tree->formula += child->formula + (tree->type == AND ? '*' : '+');
        tree->formula.back() = ')';
    };
    make_formula(tree);
    return *tree;
}

std::string Logic::to_formula(const Tree& tree) {
    return Logic::to_tree(tree.formula).formula;
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
