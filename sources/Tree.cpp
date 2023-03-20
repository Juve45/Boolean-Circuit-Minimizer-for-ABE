#include "../headers/Tree.h"
#include "../headers/debug.h"

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
                                    product->edges.push_back(factor);
                                terms.push_back(product);
                                factors.clear();
                                factors.push_back(operands[i]);
                            }
                        Tree *product = new Tree(AND);
                        for (Tree* factor : factors)
                            product->edges.push_back(factor);
                        terms.push_back(product);
                        Tree *sum = new Tree(OR);
                        for (Tree* term : terms)
                            sum->edges.push_back(term);
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
    function<void(Tree*)> clean = [&](Tree* tree) {
        for (Tree* child : tree->edges)
            clean(child);
        if (tree->edges.size() == 1) {
            Tree *child = tree->edges.back();
            tree->edges.pop_back();
            for (Tree* grand : child->edges)
                tree->edges.push_back(grand);
            tree->node_type = child->node_type;
            tree->formula = child->formula;
            delete child;
        }
    };
    clean(tree);

    function<void(Tree*)> make_formula = [&](Tree* tree) {
        for (Tree* child : tree->edges) {
            make_formula(child);
            child->parent = tree;
        }
        if (tree->edges.empty()) return;
        tree->formula = "(";
        for (Tree* child : tree->edges)
            tree->formula += child->formula + (tree->node_type == AND ? '*' : '+');
        tree->formula.back() = ')';
    };
    make_formula(tree);
    return *tree;
}

std::ostream& operator<<(std::ostream& out, const Tree& tree) {
    std::function<void(Tree, int)> dfs = [&](Tree node, int level) {
        for (int i = 0; i < level; i++)
            std::cout << "  ";
        std::cout << (node.node_type == AND ? "AND" : node.node_type == OR ? "OR" : "INPUT") << ' ' << node.formula << '\n';
        for (Tree* child : node.edges)
            dfs(*child, level + 1);
    };
    dfs(tree, 0);
    return out;
}
