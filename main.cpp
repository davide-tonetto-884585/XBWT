#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <chrono>

#include "LabeledTree/LabeledTree.hpp"
#include "XBWT/XBWT.hpp"

using namespace std;

int main()
{
    bool verbose = true;

    // Stringa di input
    //string str = "(A(B(D(a))(a)(E(b)))(C(D(c))(b)(D(c)))(B(D(b))))";
    //string str = "(A(D(D(D(E(D(E(c)))(c)))(E(c)))(E(c))))";
    string str = "(A(B(D(C(C(b))(a)(B(a)))(D(E(c)))(D(B(a))(a)(B(c)))(a))(E(B(D(a))(a)(E(C(C(C(C(b))(a)(B(a)))(D(E(c)))(D(B(a))(a)(B(c)))(b))(a)(B(a)))(D(E(c)))(D(B(a))(a)(B(c)))(b)))(C(D(c))(b)(D(c)))(B(D(b)))(B(D(B(D(a))(a)(E(b)))(C(D(c))(b)(D(c)))(B(D(b)))(a))(a)(E(b)))(C(D(c))(b)(D(c)))(B(D(b)))(b)))(C(D(c))(b)(D(c)))(B(D(b))))";
    // string str = "(A(B(C(B(C(B(C(B(a)(b)(c)))))(b)(c)))))";
    // string str = "(a(b(c(b(c(x)(y)(z))))))";

    if (verbose)
        cout << "Input string: " << str << endl;

    auto start = chrono::high_resolution_clock::now();
    LabeledTree<char> tree(str, [](const string &label) -> char
                           { return label[0]; });
    auto end = chrono::high_resolution_clock::now();
    cout << "Tree construction time: "
         << chrono::duration_cast<chrono::microseconds>(end - start).count()
         << " ms" << endl;

    cout << "Number of nodes: " << tree.getNodes().size() << endl;

    if (verbose)
        cout << "Tree: " << tree.toString() << endl;

    // build an XBWT
    vector<unsigned int> intNodesPosSorted{};
    vector<unsigned int> testIntNodesPosSorted{};

    // Measure time for XBWT construction
    start = chrono::high_resolution_clock::now();
    XBWT<char> xbwt(tree, true, verbose, &intNodesPosSorted);
    end = chrono::high_resolution_clock::now();
    cout << endl
         << "XBWT construction time: "
         << chrono::duration_cast<chrono::microseconds>(end - start).count()
         << " ms" << endl;

    start = chrono::high_resolution_clock::now();
    XBWT<char> xbwt2(tree, false, verbose, &testIntNodesPosSorted);
    end = chrono::high_resolution_clock::now();
    cout << endl
         << "XBWT construction time no path sort: "
         << chrono::duration_cast<chrono::microseconds>(end - start).count()
         << " ms" << endl;

    cout << endl
         << "Cardinality of the alphabet: " << xbwt.getCardSigma() << endl;

    cout << "\n______________________________________________________\n"
         << endl;
    cout << "Testing construction of XBWT:" << endl;
    string errors = "";

    if (verbose)
    {
        cout << "IntNodesPosSorted:\t";
        for (unsigned int i : intNodesPosSorted)
            cout << i << " ";
        cout << endl;

        cout << "TestIntNodesPosSorted:\t";
        for (unsigned int i : testIntNodesPosSorted)
            cout << i << " ";
        cout << endl;
    }

    if (intNodesPosSorted != testIntNodesPosSorted)
        throw std::runtime_error("IntNodesPosSorted is not correct");

    // rebuild the tree
    cout << endl
         << "Testing rebuildTree method:" << endl;
    start = chrono::high_resolution_clock::now();
    auto rebuiltTree = xbwt.rebuildTree();
    end = chrono::high_resolution_clock::now();
    cout << "Rebuild tree time: "
         << chrono::duration_cast<chrono::microseconds>(end - start).count()
         << " ms" << endl;

    if (verbose)
        cout << "Rebuilded tree: " << rebuiltTree.toString() << endl;

    if (rebuiltTree.toString() != tree.toString())
        errors += "Rebuilded tree is not correct\n";

    cout << endl
         << "Testing navigation methods..." << endl;
    unsigned int treeSize = tree.getNodes().size();
    vector<unsigned int> leafNodes{};
    for (unsigned int i = 0; i < treeSize; ++i)
    {
        auto children = xbwt.getChildren(i);
        if (verbose)
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
            if (verbose)
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
                if (verbose)
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
            if (verbose)
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

    cout << endl
         << "Testing subPathSearch method..." << endl;
    for (unsigned int i = 0; i < treeSize; ++i)
    {
        auto upwardPath = xbwt.getUpwardPath(i);
        if (verbose)
            cout << "Get upward path of " << i << ": " << string(upwardPath.begin(), upwardPath.end()) << endl;

        for (unsigned int j = 0; j < upwardPath.size(); ++j)
        {
            std::vector<char> path(upwardPath.begin() + j, upwardPath.end());
            auto subPathSearchResult = xbwt.subPathSearch(path);
            if (verbose)
                cout << "\tSubpath search of " << std::string(upwardPath.begin() + j, upwardPath.end()) << ": " << subPathSearchResult.first << " " << subPathSearchResult.second << endl; 

            if (subPathSearchResult.first != -1 && subPathSearchResult.second != -1)
            {
                for (unsigned int k = subPathSearchResult.first; k <= subPathSearchResult.second; ++k)
                {
                    auto checkUpwardPath = xbwt.getUpwardPath(k);
                    if (std::string(checkUpwardPath.begin(), checkUpwardPath.end()).find(std::string(upwardPath.begin() + j, upwardPath.end())) == std::string::npos)
                        errors += "Subpath search is not correct\n";
                }
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