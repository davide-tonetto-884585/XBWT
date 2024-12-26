#include <iostream>
#include <vector>
#include <string>

#include "LabeledTree/LabeledTree.hpp"
#include "XBWT/XBWT.hpp"

using namespace std;

int main() {
    Node<unsigned int> root(1);
    Node<unsigned int> node3(3);

    root.addChild(3);

    // build a labeled tree
    string str = "(1(2(4)(5))(32(6)(7)))";
    cout << "String: " << str << endl;

    LabeledTree<unsigned int> tree(str);
    cout << "Tree: " << tree.toString() << endl;

    // build an XBWT
    XBWT xbwt(tree);

    return 0;
}