#include "../headers/Tree.h"

Tree& Tree::from(std::string formula) {
    std::stack<char> operatorStack;
    std::stack<Tree*> operandStack;
    for (const char chr : formula)
        if ('a' <= chr && chr <= 'z') {
            Tree *node = new Tree;
            node->node_type = INPUT;
            node->formula = std::string(1, chr);
            operandStack.push(node);
        }
        else {
            operatorStack.push(chr);
            if (chr == ')') {
                char operation;
                std::vector<Tree*> operands;
                while (true) {
                    const char topOperator = operatorStack.top();
                    operatorStack.pop();
                    if (topOperator == '(') break;
                    if (topOperator == '*' || topOperator == '+') operation = topOperator;
                    Tree *topOperand = operandStack.top();
                    operandStack.pop();
                    operands.push_back(topOperand);
                }
                std::sort(operands.begin(), operands.end(), [&](Tree* a, Tree* b) {
                    return a->formula < b->formula;
                });
                Tree *node = new Tree;
                node->node_type = operation == '+' ? OR : AND;
                if (operation == '+') node->formula += '(';
                for (Tree* operand : operands) {
                    operand->parent = node;
                    node->edges.push_back(operand);
                    node->formula += operand->formula + (operation == '+' ? "+" : "");
                }
                if (operation == '+') node->formula.pop_back();
                if (operation == '+') node->formula += ')';
                operandStack.push(node);
            }
        }
    operandStack.top()->parent = nullptr;
    return *operandStack.top();
}

std::ostream& operator<<(std::ostream& out, const Tree& tree) {
    std::function<void(Tree)> dfs = [&](Tree node) {
        for (Tree* edge : node.edges) {
            out << (node.node_type == AND ? "AND" : node.node_type == OR ? "OR" : "INPUT") << ' ' << node.formula << ' ' << edge->formula << '\n';
            dfs(*edge);
        }
    };
    dfs(tree);
    return out;
}
