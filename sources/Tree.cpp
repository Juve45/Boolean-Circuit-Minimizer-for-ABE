#include "../headers/abeai.h"
#include "../headers/debug.h"

void Tree::update_formula() {
    std::set<std::string> subformulas;
    formula = "";
    for (Tree* child : children)
        subformulas.insert(child->formula);
    if (type == AND)
        for (const auto& subformula : subformulas)
            formula += subformula;
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

void Tree::trim() {
    if (parent && type != INPUT) {
        if (children.empty())
            parent->erase_child(this);
        if (children.size() == 1) {
            Tree *child = children.front();
            if (child->type != INPUT) {
                assert(parent->type == child->type);
                for (Tree* child_child : child->children)
                    parent->add_child(child_child);
            }
            else
                parent->add_child(child);
            parent->erase_child(this);
        }
    }
    for (Tree* child : children)
        child->trim();
}
