#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include "LabeledTree/LabeledTree.hpp"
#include "XBWT/XBWT.hpp"

using namespace std;

int main()
{
    // Stringa di input
    // string str = "(A(C(C(b))(a)(B(a)))(D(E(c)))(D(B(a))(a)(B(c))))";
    string str = "(A(B(D(C(C(b))(a)(B(a)))(D(E(c)))(D(B(a))(a)(B(c)))(a))(E(B(D(a))(a)(E(C(C(C(C(b))(a)(B(a)))(D(E(c)))(D(B(a))(a)(B(c)))(b))(a)(B(a)))(D(E(c)))(D(B(a))(a)(B(c)))(b)))(C(D(c))(b)(D(c)))(B(D(b)))(B(D(B(D(a))(a)(E(b)))(C(D(c))(b)(D(c)))(B(D(b)))(a))(a)(E(b)))(C(D(c))(b)(D(c)))(B(D(b)))(b)))(C(D(c))(b)(D(c)))(B(D(b))))";
    // string str = "(a(b(c(b(c(b(c(b(x)(y)(z)))))(y)(z)))))";
    // string str = "(a(b(c(b(c(x)(y)(z))))))";
    // string str = "(1(2(1(2)(2(3)))(2)))";

    cout << "Input string: " << str << endl;

    // Crea l'albero etichettato utilizzando la stringa convertita
    LabeledTree<char> tree(str, [](const string &label) -> char { return label[0]; });
    cout << "Tree: " << tree.toString() << endl;

    // build an XBWT
    vector<unsigned int> intNodesPosSorted{};
    XBWT<char> xbwt(tree, false, true, &intNodesPosSorted);
    string errors = "";

    cout << "\n______________________________________________________" << endl;
    cout << "Testing construction of XBWT:" << endl;
    vector<unsigned int> testIntNodesPosSorted = xbwt.upwardStableSortConstruction(tree);

    cout << "IntNodesPosSorted:\t";
    for (unsigned int i : intNodesPosSorted)
        cout << i << " ";
    cout << endl;

    cout << "TestIntNodesPosSorted:\t";
    for (unsigned int i : testIntNodesPosSorted)
        cout << i << " ";
    cout << endl;

    if (intNodesPosSorted != testIntNodesPosSorted)
        throw std::runtime_error("IntNodesPosSorted is not correct");

    // rebuild the tree
    cout << "Testing rebuildTree method:" << endl;
    auto rebuiltTree = xbwt.rebuildTree();
    cout << "Rebuilded tree: " << rebuiltTree.toString() << endl;
    if (rebuiltTree.toString() != tree.toString())
        errors += "Rebuilded tree is not correct\n";

    cout << "Testing navigation methods:" << endl;
    unsigned int treeSize = tree.getNodes().size();
    vector<unsigned int> leafNodes{};
    for (unsigned int i = 0; i < treeSize; ++i)
    {
        auto children = xbwt.getChildren(i);
        cout << "Get children of " << i << ": " << children.first << " " << children.second << endl;

        if (children.first > children.second)
        {
            errors += "Children are invalid\n";
            break;
        }

        if (children.first >= treeSize || children.second >= treeSize)
        {
            errors += "Children are out of bounds\n";
            break;
        }

        if (children.first == -1 && children.second == -1)
        {
            leafNodes.push_back(i);
            continue;
        }

        vector<unsigned int> childrenLabels{};
        vector<unsigned int> labelCounts{};
        for (unsigned int k = 1; k <= children.second - children.first + 1; ++k)
        {
            auto rankedChild = xbwt.getRankedChild(i, k);
            cout << "\tGet ranked child of " << i << " with k = " << k << ": " << rankedChild << endl;
            if (rankedChild != children.first + k - 1)
                errors += "Ranked child is not correct\n";

            auto nodeLabel = xbwt.getNodeLabel(rankedChild);
            auto findLabelResult = find(childrenLabels.begin(), childrenLabels.end(), nodeLabel);
            if (findLabelResult == childrenLabels.end())
            {
                childrenLabels.push_back(nodeLabel);
                labelCounts.push_back(1);
            }
            else
            {
                ++labelCounts[findLabelResult - childrenLabels.begin()];
            }
        }

        vector<unsigned int> newChildren{};
        for (unsigned int k = 0; k < childrenLabels.size(); ++k)
        {
            for (unsigned int l = 0; l < labelCounts[k]; ++l)
            {
                auto charRankedChild = xbwt.getCharRankedChild(i, childrenLabels[k], l + 1);
                newChildren.push_back(charRankedChild);
                cout << "\tGet char ranked child of " << i << " with c = " << childrenLabels[k] << " and k = " << l + 1 << ": " << charRankedChild << endl;
            }
        }

        sort(newChildren.begin(), newChildren.end());
        vector<unsigned int> correctChildren{};
        for (unsigned int k = children.first; k <= children.second; ++k)
            correctChildren.push_back(k);
        if (newChildren != correctChildren)
            errors += "Char ranked children are not correct\n";

        for (unsigned int k = children.first; k <= children.second; ++k)
        {
            auto parent = xbwt.getParent(k);
            cout << "\tGet parent of " << k << ": " << parent << endl;
            if (parent != i)
                errors += "Parent is not correct\n";

            if (find(leafNodes.begin(), leafNodes.end(), parent) == leafNodes.end())
            {
                auto [first, last] = xbwt.getChildren(parent);
                if (first == -1 || last == -1)
                    errors += "Children are not correct\n";
            }
        }
    }

    cout << "\nTesting subPathSearch method:" << endl;
    for (unsigned int i = 0; i < treeSize; ++i)
    {
        auto upwardPath = xbwt.getUpwardPath(i);
        cout << "Get upward path of " << i << ": " << string(upwardPath.begin(), upwardPath.end()) << endl;

        for (unsigned int j = 0; j < upwardPath.size(); ++j)
        {
            std::vector<char> path(upwardPath.begin() + j, upwardPath.end());
            auto subPathSearchResult = xbwt.subPathSearch(path);
            cout << "\tSubpath search of " << std::string(upwardPath.begin() + j, upwardPath.end()) << ": " << subPathSearchResult.first << " " << subPathSearchResult.second << endl;

            for (unsigned int k = subPathSearchResult.first; k <= subPathSearchResult.second; ++k)
            {
                auto checkUpwardPath = xbwt.getUpwardPath(k);
                if (std::string(checkUpwardPath.begin(), checkUpwardPath.end()).find(std::string(upwardPath.begin() + j, upwardPath.end())) == std::string::npos)
                    errors += "Subpath search is not correct\n";
            }
        }
    }

    if (errors != "")
    {
        cerr << "______________________________________________________" << endl
             << "Errors:\n"
             << endl;
        cerr << errors;
    }
    else
    {
        cout << "______________________________________________________" << endl;
        cout << "All tests passed successfully!" << endl;
    }

    return 0;
}