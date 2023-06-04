#include "../headers/abeai.h"
#include "../headers/debug.h"

// Search common factors for operation type.
std::vector<std::vector<Tree*>> Factorizer::reduce(Tree* t, NodeType type) {
    std::vector<std::vector<Tree*>> ans;
    if (t->type == type) {
        std::map<std::string, std::vector<Tree*>> similar;
        for (Tree* i : t->children) {
            for (Tree* j : i->children) {
                if (similar.count(j->formula)) {
                    if (similar[j->formula].back()->parent->type == type)
                        assert(j->parent->parent == similar[j->formula].back()->parent);
                    else 
                        assert(j->parent->parent == similar[j->formula].back()->parent->parent);
                    // if (similar[j->formula].back()->parent == j->parent){
                    //     assert(j != similar[j->formula].back());
                    //     dbg(j->formula);
                    //     throw std::runtime_error("Two siblings with same parent! Please implement absorbtion");
                    // }
                }
                else
                    similar[j->formula] = std::vector<Tree*>();
                similar[j->formula].push_back(j);
            }
            if (i->type == INPUT) {
                if (similar.count(i->formula)) {
                    if (similar[i->formula].back()->parent->type == type)
                        assert(i->parent == similar[i->formula].back()->parent);
                    else 
                        assert(i->parent == similar[i->formula].back()->parent->parent);
                    // if (similar[i->formula].back()->parent == i){
                    //     assert(j != similar[j->formula].back());
                    //     dbg(j->formula);
                    //     throw std::runtime_error("Two siblings with same parent! Please implement absorbtion");
                    // }
                }
                else
                    similar[i->formula] = std::vector<Tree*>();
                similar[i->formula].push_back(i);
            }
        }
        for (const auto& i : similar)
            if (i.second.size() > 1)
                ans.push_back(i.second);
    }
    for (Tree* i : t->children) {
        const auto ans_child = reduce(i, type);
        for (const auto& i : ans_child)
            ans.push_back(i);
    }
    return ans;
}

// t1 is absorbing element
// (a+ab+cd) = (a+cd)
void Factorizer::factorize_absorption(Tree* t1, Tree* t2) {
    assert(t1->parent);
    assert(t2->parent);
    assert(t2->parent->parent);
    assert(t1->parent == t2->parent->parent);

    Tree *old2 = t2->parent;
    Tree *parent = t1->parent;

    parent->erase_child(old2);
    old2->erase();
    delete old2;

    Tree *temp = parent;
    Tree *root = parent;
    while (temp) {
        temp->update_formula();
        root = temp;
        temp = temp->parent;
    }
    root->trim();
}

void Factorizer::factorize(Tree* t1, Tree* t2) {
    assert(t1->parent);
    assert(t2->parent);
    assert(t1->parent != t2->parent);

    // absorption case
    if (t1->parent == t2->parent->parent) {
        factorize_absorption(t1, t2);
        return;
    } else if (t2->parent == t1->parent->parent) {
        factorize_absorption(t2, t1);
        return;
    }

    assert(t1->parent->parent);
    assert(t2->parent->parent);
    assert(t1->parent->parent == t2->parent->parent);

    // (ab+ac) => a(b+c)
    // and_node is the node between a and (b+c)
    // or_node is the or node from b+c
    // If we factorize (a+b)(a+c) the and and or nodes will actually have 
    // the opposit operation. 
    Tree *and_node = new Tree(t1->parent->type);
    Tree *or_node = new Tree(t1->parent->parent->type);
    Tree *old1 = t1->parent;
    Tree *old2 = t2->parent;
    Tree *parent = old1->parent;

    parent->erase_child(old1);
    parent->erase_child(old2);
    old1->erase_child(t1);
    old2->erase_child(t2);

    or_node->add_child(old1);
    or_node->add_child(old2);
    and_node->add_child(or_node);
    and_node->add_child(t1);
    
    t2->erase();
    delete t2;

    parent->add_child(and_node);

    Tree *temp = old2;
    Tree *root = old2;
    while (temp) {
        temp->update_formula();
        root = temp;
        temp = temp->parent;
    }

    root->trim();
}

void Factorizer::defactorize(Tree* t1, Tree* t2) {
    assert(t1->parent == t2->parent);
    assert(t1 != t2);
    assert(t1->type == OR);
    assert(t2->type == OR || t2->type == INPUT);

    Tree *parent = t1->parent;
    Tree *or_node = new Tree(OR);
    parent->erase_child(t1);
    parent->erase_child(t2);
    
    if (t2->type == OR) {
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
    }
    else {
        // t2->type == INPUT
        for (Tree* i : t1->children) {
            Tree *temp = new Tree(AND);
            if (!temp->has_child(i->formula)){
                temp->add_child(i->deep_copy());
            }
            if (!temp->has_child(t2->formula)) {
                temp->add_child(t2->deep_copy());
            }
            temp->update_formula();
            if (!or_node->has_child(temp->formula)) {
                or_node->add_child(temp);
            }
        }
    }

    or_node->update_formula();
    if (!parent->has_child(or_node->formula)){
        parent->add_child(or_node);
    }

    Tree *temp = or_node;
    while (temp) {
        temp->update_formula();
        temp = temp->parent;
    }
    if (parent->parent)
        parent->parent->trim();
    else
        parent->trim();
    
    t1->erase();
    delete t1;
    t2->erase();
    delete t2;
}
