#include "../headers/abeai.h"
#include "../headers/debug.h"

std::vector<std::vector<Tree*>> Factorizer::reduce(Tree* t) {
    std::vector<std::vector<Tree*>> ans;
    if (t->type == OR) {
        std::map<std::string, std::vector<Tree*>> similar;
        for (Tree* i : t->children)
            for (Tree* j : i->children) {
                if (similar.count(j->formula)) {
                    assert(j->parent->parent == similar[j->formula].back()->parent->parent);
                    if (similar[j->formula].back()->parent == j->parent){
                        assert(j != similar[j->formula].back());
                        dbg(j->formula);
                        throw std::runtime_error("Two siblings with same parent! Please implement absorbtion");
                    }
                }
                else
                    similar[j->formula] = std::vector<Tree*>();
                similar[j->formula].push_back(j);
            }
        for (const auto& i : similar)
            if (i.second.size() > 1)
                ans.push_back(i.second);
    }
    for (Tree* i : t->children) {
        const auto ans_child = reduce(i);
        for (const auto& i : ans_child)
            ans.push_back(i);
    }
    return ans;
}

void Factorizer::factorize(Tree* t1, Tree* t2) {
    assert(t1->parent);
    assert(t2->parent);
    assert(t1->parent->parent);
    assert(t2->parent->parent);
    assert(t1->parent->parent == t2->parent->parent);

    Tree *and_node = new Tree(AND);
    Tree *or_node = new Tree(OR);
    Tree *old1 = t1->parent;
    Tree *old2 = t2->parent;
    Tree *parent = old1->parent;

    parent->erase_child(old1);
    parent->erase_child(old2);

    or_node->add_child(old1);
    or_node->add_child(old2);
    and_node->add_child(or_node);
    and_node->add_child(t1);
    parent->add_child(and_node);

    old1->erase_child(t1);
    old2->erase_child(t2);
    old1->update_formula();
    old2->update_formula();

    Tree *temp = old2;
    while (temp) {
        temp->update_formula();
        temp = temp->parent;
    }
    parent->trim();
}

void Factorizer::defactorize(Tree* t1, Tree* t2) {
    assert(t1->parent == t2->parent);
    assert(t1 != t2);
    assert(t1->type == OR);
    assert(t2->type == OR);

    Tree *parent = t1->parent;
    Tree *or_node = new Tree(OR);
    parent->add_child(or_node);

    for (Tree* i : t1->children)
        for (Tree* j : t2->children) {
            Tree *temp = new Tree(AND);
            if (!temp->has_child(i->formula))
                temp->add_child(i->deep_copy());
            if (!temp->has_child(j->formula))
                temp->add_child(j->deep_copy());
            temp->update_formula();
            if (!or_node->has_child(temp->formula))
                or_node->add_child(temp);
        }
    parent->erase_child(t1);
    parent->erase_child(t2);

    Tree *temp = or_node;
    while (temp) {
        temp->update_formula();
        temp = temp->parent;
    }
    if (parent->parent)
        parent->parent->trim();
    else
        parent->trim();
}
