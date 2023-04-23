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
    assert(std::find(children.begin(), children.end(), child) == children.end());
    children.push_back(child);
    child->parent = this;
    update_formula();
}

void Tree::erase_child(Tree* child) {
    auto it = std::find(children.begin(), children.end(), child);
    assert(it != children.end());

    children.erase(it);
    update_formula();
}

Tree* Tree::deep_copy() const {
    Tree* res = new Tree(type);
    res->formula = formula;
    res->parent = nullptr;
    res->children = std::vector<Tree*>();

    for (auto child : children) {
        res->children.push_back(child->deep_copy());
        res->children.back()->parent = res;
    }

    return res;
}

bool Tree::trim() {
    
    std::vector<Tree*> children_copy = children;
    for (Tree* child : children_copy) {
        if(child->trim()) 
            delete(child);
    }

    std::set<std::string> child_formulas;
    // dbg(this->children);
    for(int i = 0; i < int(children.size()); i++) {
        Tree* child = children[i];
        if (child_formulas.count(child->formula)) {
            // dbg(child);
            this->erase_child(child);
            delete child;
            i--;
        } else 
            child_formulas.insert(child->formula);
    }

    if (parent && type != INPUT) {
        if (children.empty()) {
            parent->erase_child(this);
            return true;
        }
        if (children.size() == 1) {
            Tree *child = children.front();
            parent->erase_child(this);
            if (child->type != INPUT) {
                assert(parent->type == child->type);
                for (Tree* child_child : child->children)
                    if (!parent->has_child(child_child->formula))
                        parent->add_child(child_child);
            }
            else {
                if (!parent->has_child(child->formula))
                    parent->add_child(child);
            }
            return true;
            // to do: remove this node
        }
        else if (parent->type == type) {
            parent->erase_child(this);

            for (Tree* child : children) {
                if (!parent->has_child(child->formula))
                    parent->add_child(child);
            }
            return true;
            // to do: remove this node
        }
    }
    if (!children.empty())
        update_formula();
    return false;
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


void Tree::clean() {
    for (Tree* child : this->children)
        child->clean();
    
    if (this->children.size() == 1) {
        Tree *child = this->children.back();
        this->children.pop_back();
        for (Tree* grand : child->children)
            this->children.push_back(grand);
        this->type = child->type;
        this->formula = child->formula;
        delete child;
    }
};

void Tree::erase() {
    for (Tree* child : this->children) {
        child->erase();
        delete child;
    }
}
