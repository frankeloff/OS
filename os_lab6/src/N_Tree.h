#ifndef N_TREE_H
#define N_TREE_H

#include<iostream>

template<typename T>
class N_Tree {
public:
    N_Tree() = default;

    explicit N_Tree(T dat) : data(dat), son(nullptr), brother(nullptr) {};



    void insert(T id, T parent) { // добавление узла по родителю
        if (data == parent){
            if (son == nullptr){
                son = new N_Tree<T>(id);
                return;
            }
            auto *tree = son;
            while (tree->brother != nullptr){
                tree = tree->brother;
            }
            tree->brother = new N_Tree<T>(id);
            return;
        }
        if (son != nullptr){
            son->insert(id, parent);
        }
        if (brother != nullptr){
            brother->insert(id, parent);
        }
    }

    T get_data() {
        return this->data;
    }

    void set_data(T dat) {
        this->data = dat;
    }

    bool remove(T id) {
        if (this->son->data == id) {
            this->son = nullptr;
            return true;
        }
        if (this->brother->data == id) {
            this->brother = nullptr;
            return true;
        }
        bool ok = false;
        if (this->son != nullptr) {
            ok = this->son->remove(id);
        }
        if (this->brother != nullptr) {
            ok = ok || this->brother->remove(id);
        }
        return ok;
    }

    template<typename S>
    friend N_Tree<S> *search_tree(S search, N_Tree<S> *t);
    template<typename F>
    friend void traverse_inorder(N_Tree<F> *t);
    template<typename H>
    friend void delete_topology(N_Tree<H> *t);
private:
    T data;
    N_Tree<T> *son = nullptr, *brother = nullptr;
};

template<typename T>
N_Tree<T> *search_tree(T search, N_Tree<T> *t) {// поиск узла по значению
    if (t == nullptr){
        return t;
    }
    if (t->data == search) {
        return t;
    }
    N_Tree<T> *tree = nullptr;
    if (t->son != nullptr) {
        tree = search_tree(search, t->son);
    }
    if (t->brother != nullptr and tree == nullptr ) {
        tree = search_tree(search, t->brother);
    }
    return tree;
}

template<typename F>
void traverse_inorder(N_Tree<F> *t){
    if (t->son != nullptr)
        traverse_inorder(t->son);

    std::cout << t->data << std::endl;

    if (t->brother != nullptr)
        traverse_inorder(t->brother);
}

template<typename H>
void delete_topology(N_Tree<H> *t){
    if (t->son != nullptr){
        delete_topology(t->son);
    }
    if(t->brother != nullptr){
        delete_topology(t->brother);
    }
    delete t;
}

#endif //N_TREE_H
