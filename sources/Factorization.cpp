#include "../headers/abeai.h"
#include "../headers/debug.h"

std::vector<std::vector<Tree*>> our_reduce(Tree* t) {
    std::vector<std::vector<Tree*>> ans;
    if (t->type == OR) {
        std::map<std::string, std::vector<Tree*>> similar;
        for (auto i : t->children)
            for (auto j : i->children) {
                if (similar.count(j->formula)) {
                    assert(j->parent->parent == similar[j->formula].back()->parent->parent);
                    if (similar[j->formula].back()->parent == j->parent)
                        throw std::runtime_error("Two siblings with same parent! Please implement absorbtion");
                }
                else
                    similar[j->formula] = std::vector<Tree*>();
                similar[j->formula].push_back(j);
            }
        for (auto i : similar)
            if (i.second.size() > 1)
                ans.push_back(i.second);
    }
    for (auto i : t->children) {
        auto ans_child = our_reduce(i);
        for (auto i : ans_child)
            ans.push_back(i);
    }
    return ans;
}

void erase_child(Tree* parent, Tree* child) {
    auto it = find(parent->children.begin(), parent->children.end(), child);
    if (it != parent->children.end())
        parent->children.erase(it);
    else
        assert(false);
}

void add_edge(Tree* parent, Tree* child) {
    assert(parent);
    assert(child);
    parent->children.push_back(child);
    child->parent = parent;
}

void trimm(Tree* t) {
    if (t->parent && t->type != INPUT) {
        if (t->children.empty())
            erase_child(t->parent, t);
        if (t->children.size() == 1) {
            Tree *child = t->children.front();
            if (child->type != INPUT) {
                assert(t->parent->type == child->type);
                for (auto i : child->children)
                    add_edge(t->parent, i);
            }
            else
                add_edge(t->parent, child);
            erase_child(t->parent, t);
        }
    }
    for (auto i : t->children)
        trimm(i);
}

void factorize(Tree* t1, Tree* t2) {
    assert(t1->parent);
    assert(t2->parent);
    assert(t1->parent->parent);
    assert(t2->parent->parent);
    assert(t1->parent->parent == t2->parent->parent);

    Tree *and_node = new Tree(AND);
    Tree *or_node = new Tree(OR);

    Tree *old1 = t1->parent;
    Tree *old2 = t2->parent;

    auto parent = old1->parent;

    erase_child(parent, old1);
    erase_child(parent, old2);

    add_edge(or_node, old1);
    add_edge(or_node, old2);
    add_edge(and_node, or_node);
    add_edge(and_node, t1);
    add_edge(parent, and_node);

    erase_child(old1, t1);
    erase_child(old2, t2);

    old1->compute_formula();
    old2->compute_formula();

    auto tmp = old2;
    while (tmp) {
        tmp->compute_formula();
        tmp = tmp->parent;
    }
    trimm(parent);
}

void defactorize(Tree* t1, Tree* t2) {
    assert(t1->parent == t2->parent);
    assert(t1->type == OR);
    assert(t2->type == OR);

    auto parent = t1->parent;

    Tree *or_node = new Tree(OR);
    add_edge(parent, or_node);

    for (auto i : t1->children)
        for (auto j : t2->children) {
            Tree *tmp = new Tree(AND);
            for (auto k : i->children)
                add_edge(tmp, k);
            for (auto k : j->children)
                add_edge(tmp, k);
            tmp->compute_formula();
            add_edge(or_node, tmp);
        }

    erase_child(parent, t1);
    erase_child(parent, t2);

    auto tmp = or_node;
    while (tmp) {
        tmp->compute_formula();
        tmp = tmp->parent;
    }
}
