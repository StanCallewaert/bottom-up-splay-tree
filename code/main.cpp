#include "splaytree.h"
#include "splaynode.h"

#include <cstdlib>

using namespace std;

int main(int argc, char** argv) {
    SplayTree<int> tree{};
    
    // Construct a splay tree
    // A splay operation on the newly added element is performed after each add
    tree.add(10);
    tree.add(20); // Case 1             (see Github images)
    tree.add(15); // Case 2             (see Github images)
    tree.add(5);  // Case 3             (see Github images)
    tree.add(1);  // Case 3             (see Github images)
    
    // Write the splay tree to a file
    // If you copy the text from this file (in the same directory as this cpp file)
    // Then you can paste it in the textarea of the website: http://webgraphviz.com
    // To see what the splay tree looks like
    // You can use this line to visualize the splay tree in other parts of the code as well
    tree.visualize("ConstructedSplayTree.txt");
    
    // Delete the nodes from the splay tree one by one
    // A splay operation on the parent of the physically deleted element is performed after each delete
    tree.del(20); // Case 3 and case 1  (see Github images)
    tree.del(5);  // Case 2             (see Github images)
    tree.del(10);
    tree.del(15);
    tree.del(1);
    
    return 0;
}

