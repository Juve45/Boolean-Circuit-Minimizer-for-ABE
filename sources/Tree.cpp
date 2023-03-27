#include "../headers/abeai.h"
#include "../headers/debug.h"

void Tree::compute_formula() {
    std::set<std::string> subformulas;
    formula = "";
    for (auto child : children)
        subformulas.insert(child->formula);

    if (type == AND)
        for (auto subformula : subformulas)
            formula += subformula;
    else if (type == OR) {
        formula = "";
        if (parent && subformulas.size() > 1)
            formula += '(';
        for (auto subformula : subformulas)
            formula += subformula + '+';
        formula.pop_back();
        if (parent && subformulas.size() > 1)
            formula += ')';
    }
    // TODO for INPUT?
}

Tree& Tree::from(std::string formula) {
    std::string new_formula = "(";
    for (const char chr : formula) {
        if (isalpha(new_formula.back()) && isalpha(chr))
            new_formula += '*';
        new_formula += chr;
    }
    new_formula += ')';

    std::stack<char> operatorStack;
    std::stack<Tree*> operandStack;
    for (const char chr : new_formula)
        if ('a' <= chr && chr <= 'z') {
            Tree *node = new Tree(INPUT);
            node->formula = std::string(1, chr);
            operandStack.push(node);
        }
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
