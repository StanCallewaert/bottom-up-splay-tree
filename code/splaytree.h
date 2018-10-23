#ifndef SPLAYTREE_H
#define SPLAYTREE_H

#include "splaynode.h"

#include <cstdlib>
#include <iostream>
#include <queue>
#include <memory>
#include <functional>
#include <fstream>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <string>

using std::unique_ptr;
using std::make_unique;
using std::string;
using std::move;
using std::ostream;
using std::ofstream;
using std::ostringstream;
using std::cout;
using std::endl;
using std::swap;

template <class Key>
class SplayNode;

template <class Key>
class SplayTree : public unique_ptr<SplayNode<Key>> {
    public:
        // Use unique pointer methods (default constructor of unique_ptr is used for this class)
        using unique_ptr<SplayNode<Key>>::unique_ptr;
        
        // Use the move operator of unique_ptr<SplayNode<Key>> as the move operator from SplayTree
        SplayTree(unique_ptr<SplayNode<Key>>&& tree) : unique_ptr<SplayNode<Key>> (move(tree)) { }
        
        void rotate(bool left);
        void add(const Key& key);
        void del(const Key& key);

        // Use this function to visualize the current graph
        // Just copy the textual output that is in the given file after executing the program
        // and paste it in the textarea of the website: http://webgraphviz.com
        void visualize(const char* filename) const;
    protected:
        // Returns the pointer to the node (and his parent) that was searched for
        void search(const Key& key, SplayTree<Key>*& newNodePtr, SplayNode<Key>*& parent);
        
        // Search pointer to successor of current node
        SplayTree<Key>* searchDeletableSuccessor();
        
        // Recursive method that makes the node where it is called upon root
        void splay(SplayTree<Key>* root);

        // Recursively print out the text of each node to be able to visualize the tree
        string visualizeNodesRecusively(ostream& out, int& nullCounter) const;
};

template <class Key>
void SplayTree<Key>::rotate(bool left) {
    // Get child of root
    SplayTree<Key> child = move((*this)->returnChild(!left));
    
    // Move beta (image in readme.md)
    (*this)->returnChild(!left) = move(child->returnChild(left));
    
    // Move root (image in readme.md)
    child->returnChild(left) = move(*this);
    
    // Move child (image in readme.md)
    *this = move(child);
    
    // Restore parent pointers
    (*this)->parent = (*this)->returnChild(left)->parent;
    (*this)->returnChild(left)->parent = this->get();
    
    // If alpha is not null, restore its parent pointer as well
    if ((*this)->returnChild(left)->returnChild(!left)) {
        (*this)->returnChild(left)->returnChild(!left)->parent = (*this)->returnChild(left).get();
    }
}

template <class Key>
void SplayTree<Key>::add(const Key& key) {
    // Initialize newNodePtr and parent to get their real values in the search function
    SplayTree<Key>* newNodePtr;
    SplayNode<Key>* parent;
    
    // Find the spot (and his parent) where the new node has to be added to
    this->search(key, newNodePtr, parent);
    
    // Only add the new key if it does not exist in the splay tree yet
    if (!*newNodePtr) {
        SplayTree<Key> splayTree = make_unique<SplayNode<Key>>(key);
        *newNodePtr = move(splayTree);
        (*newNodePtr)->parent = parent;
    }
    
    // Bottom-up splay operation
    newNodePtr->splay(this);
}

template <class Key>
void SplayTree<Key>::del(const Key& key) {
    // Initialize nodePtr and parent to get their real values in the search function
    SplayTree<Key>* nodePtr;
    SplayNode<Key>* parent;
    
    // Find the spot (and his parent) where the new node has to be added to
    this->search(key, nodePtr, parent);
    
    // Only delete the node if it already exists in the red-black tree
    if (*nodePtr) {
        SplayTree<Key>* successor = nodePtr->searchDeletableSuccessor();
        swap((*nodePtr)->key, (*successor)->key);
        
        SplayTree<Key>* parentTree = (*successor)->returnParent(this);
        
        if ((*successor)->left) {
            *successor = move((*successor)->left);
        } else {
            *successor = move((*successor)->right);
        }

        // Bottom-up splay operation
        if (parentTree && *parentTree) {
            parentTree->splay(this);
        }
    } else {
        SplayTree<Key>* grandparent = parent->returnParent(this);
        SplayTree<Key>* parentTree = this;
        
        if (grandparent) {
            parentTree = &((*grandparent)->returnChild((*grandparent)->isLeftChild(parent)));
        }
        
        // Bottom-up splay operation
        parentTree->splay(this);
    }
}

template <class Key>
void SplayTree<Key>::search(const Key& key, SplayTree<Key>*& newNodePtr, SplayNode<Key>*& parent) {
    newNodePtr = this;
    parent = nullptr;
    
    while (*newNodePtr && (*newNodePtr)->key !=key) {
        parent = newNodePtr->get();
        newNodePtr = &((*newNodePtr)->returnChild(((*newNodePtr)->key > key)));
    }
}

template <class Key>
SplayTree<Key>* SplayTree<Key>::searchDeletableSuccessor() {
    if ((*this)->right && (*this)->left) {
        SplayTree<Key>* successor = &((*this)->right);
        
        while ((*successor)->left && (*successor)->right) {
           successor = &((*successor)->left);
        }
        
        return successor;
    }
    
    return this;
}

template <class Key>
void SplayTree<Key>::splay(SplayTree<Key>* root) {
    SplayTree<Key>* parent = (*this)->returnParent(root);
    
    if (parent) {
        cout << "Parent " << (*this)->key << endl;
        bool isLeftChildOfParent = (*parent)->isLeftChild(&(**this));
        SplayTree<Key>* grandparent = (*parent)->returnParent(root);
        
        cout << "After parent" << endl;
        
        if (grandparent) {
            bool isLeftChildOfGrandparent = (*grandparent)->isLeftChild(&(**parent));
            cout << "After grandparent" << endl;
            
            if (isLeftChildOfParent == isLeftChildOfGrandparent) {
                grandparent->rotate(!isLeftChildOfGrandparent);
                grandparent->rotate(!isLeftChildOfGrandparent);
            } else {
                cout << "In" << endl;
                parent->rotate(!isLeftChildOfParent);
                grandparent->rotate(!isLeftChildOfGrandparent);
            }
            
            // Recursively splay
            grandparent->splay(root);
        } else {
            parent->rotate(!isLeftChildOfParent);
        }
    }
}

template <class Key>
void SplayTree<Key>::visualize(const char* filename) const {
    ofstream out(filename);
    assert(out);
    
    // Nullptr nodes need to get their own number, hence the nullcounter
    int nullCounter = 0;
    out<<"digraph {\n";
    
    // Recursively print out the text of each node to be able to visualize the tree
    this->visualizeNodesRecusively(out, nullCounter);
    
    out<<"}";
}

template <class Key>
string SplayTree<Key>::visualizeNodesRecusively(ostream& out, int& nullCounter) const {
    ostringstream SplayTreeString;
    
    if (!*this){
        SplayTreeString << "null" << ++nullCounter;
        out << SplayTreeString.str() << " [shape=point];\n";
    }
    else{
        SplayTreeString << '"' << (*this)->key<<'"';
        out << SplayTreeString.str() << "[label=\"" << (*this)->key << "\"]";
        out<<";\n";
        string leftChild = (*this)->left.visualizeNodesRecusively(out, nullCounter);
        string rightChild = (*this)->right.visualizeNodesRecusively(out, nullCounter);
        out << SplayTreeString.str() << " -> " << leftChild << ";\n";
        out << SplayTreeString.str() << " -> " << rightChild << ";\n";
    };
    
    return SplayTreeString.str();
}

#endif /* SPLAYTREE_H */
