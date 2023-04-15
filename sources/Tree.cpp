#include "../headers/abeai.h"
#include "../headers/debug.h"


bool has_or_child(const std::string &subformula) {
    int nested_level = 0;
    for (const char& c : subformula) {
        if (c == '(') nested_level++;
        else if (c == '+' && nested_level == 0) {
            return true;
        } 
        else if (c == ')') nested_level--;
    }
    return false;
}

void Tree::update_formula() {
    std::set<std::string> subformulas;
    formula = "";
    for (Tree* child : children)
        subformulas.insert(child->formula);
    if (type == AND)
        for (const auto& subformula : subformulas) {
            bool _has_or_child = has_or_child(subformula);
            if (_has_or_child) {
                formula += "(";
            }
            formula += subformula;
            if (_has_or_child) {
                formula += ")";
            }
        }
    else if (type == OR) {
        formula = "";
        if (parent && subformulas.size() > 1)
            formula += '(';
        for (const auto& subformula : subformulas)
            formula += subformula + '+';
        formula.pop_back();
        if (parent && subformulas.size() > 1)
            formula += ')';
    }
}

void Tree::add_child(Tree* child) {
    assert(child);
    children.push_back(child);
    child->parent = this;
}

void Tree::erase_child(Tree* child) {
    const auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end())
        children.erase(it);
    else
        assert(false);
}

Tree* Tree::deep_copy() const {
    Tree* res = new Tree(type);
    res->formula = formula;
    res->parent = nullptr;
    res->children = std::vector<Tree*>();

    for (auto child : children) {
        res->children.push_back(child->deep_copy());
    }

    return res;
}

void Tree::trim() {
    std::vector<Tree*> children_copy = children;
    for (Tree* child : children_copy)
        child->trim();

    if (parent && type != INPUT) {
        if (children.empty())
            parent->erase_child(this);
        if (children.size() == 1) {
            Tree *child = children.front();
            parent->erase_child(this);
            if (child->type != INPUT) {
                assert(parent->type == child->type);
                for (Tree* child_child : child->children)
                    if (!parent->has_child(child->formula))
                        parent->add_child(child_child);
            }
            else
                if (!parent->has_child(child->formula))
                    parent->add_child(child);
            // to do: remove this node
        }
        else if (parent->type == type) {
            for (Tree* child : children) {
                if (!parent->has_child(child->formula))
                    parent->add_child(child);
            }
            parent->erase_child(this);
            // to do: remove this node
        }
    }
    if (!children.empty())
        update_formula();
}

bool Tree::has_child(const std::string& formula) const {
    for (Tree* child : children)
        if (child->formula == formula)
            return true;
    return false;
}

int Tree::get_cost() const {
    int cost = 0;
    for (auto c : formula) {
        if (isalpha(c)) {
            cost++;
        }
    }
    return cost;
}
