#ifndef SPLAYNODE_H
#define SPLAYNODE_H

#include "splaytree.h"

template <class Key>
class SplayTree;

template <class Key>
class SplayNode {
    friend class SplayTree<Key>;
    public:
        SplayNode():parent(0){}
        SplayNode(const Key& key):key{key},parent(0){};
        SplayNode(Key&& key):key{move(key)},parent(0){};
        
        SplayTree<Key>& returnChild(const bool left);
        bool isLeftChild(SplayNode<Key>* node);
        SplayTree<Key>* returnParent(SplayTree<Key>* root);
        
        Key key;
        SplayNode<Key>* parent;
        SplayTree<Key> left, right;
};

template <class Key>
SplayTree<Key>& SplayNode<Key>::returnChild(const bool left){
    return left ? this->left : this->right;
}

template <class Key>
bool SplayNode<Key>::isLeftChild(SplayNode<Key>* node) {
    return &(*(this->left)) == node;
}

template <class Key>
SplayTree<Key>* SplayNode<Key>::returnParent(SplayTree<Key>* root) {
    SplayNode<Key>* parentNode = this->parent;
    
    if (parentNode) {
        SplayNode<Key>* grandparentNode = parentNode->parent;
        
        if (grandparentNode) {
            return &(grandparentNode->returnChild(grandparentNode->isLeftChild(parentNode)));
        }
        
        return root;
    }
    
    return nullptr;
}

#endif /* SPLAYNODE_H */
