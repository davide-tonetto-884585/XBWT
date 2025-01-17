#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include "LabeledTree/LabeledTree.hpp"
#include "XBWT/XBWT.hpp"

using namespace std;

std::map<std::string, unsigned int> labelToInt(std::vector<std::string> &labels)
{
    std::map<std::string, unsigned int> labelMap;

    // sort the labels
    std::sort(labels.begin(), labels.end());

    unsigned int currentInt = 1;
    for (const std::string &label : labels)
    {
        if (labelMap.find(label) == labelMap.end())
        {
            labelMap[label] = currentInt++;
        }
    }

    return labelMap;
}

std::vector<std::string> getLabels(const std::string &str, unsigned int &cardSigma, unsigned int &cardSigmaN)
{
    std::vector<std::string> labels{};
    std::vector<std::string> internalLabels{};
    std::string label = "";
    unsigned int i = 0;
    for (char ch : str)
    {
        if (ch == '(' or ch == ')')
        {
            if (!label.empty())
            {
                // add the label to the vector if it is not already present
                if (std::find(labels.begin(), labels.end(), label) == labels.end())
                {
                    labels.push_back(label);
                    ++cardSigma;
                }

                if (!(ch == ')' and str[i - 2] == '('))
                {
                    if (std::find(internalLabels.begin(), internalLabels.end(), label) == internalLabels.end())
                    {
                        internalLabels.push_back(label);
                        ++cardSigmaN;
                    }
                }

                label = "";
            }
        }
        else
        {
            label += ch;
        }

        ++i;
    }

    return labels;
}

int main()
{
    // Stringa di input
    // string str = "(A(C(C(b))(a)(B(a)))(D(E(c)))(D(B(a))(a)(B(c))))";
    // string str = "(A(B(D(a))(a(a(b(c(b(c(b(c(b(x)(y)(z)))))(y)(z))))))(E(B(D(a))(a)(E(b)))(C(D(c))(b)(D(c)))(B(D(b)))(B(D(B(D(a))(a)(E(b)))(C(D(c))(b)(D(c)))(B(D(b)))(a))(a)(E(b)))(C(D(c))(b)(D(c)))(B(D(b)))(b)))(C(D(c))(b)(D(c)))(B(D(b))))";
    string str = "(a(b(c(b(c(b(c(b(x)(y)(z)))))(y)(z)))))";
    // string str = "(a(b(c(b(c(x)(y)(z))))))";
    // string str = "(1(2(1(2)(2(3)))(2)))";

    cout << "Input string: " << str << endl;

    unsigned int cardSigma = 0;
    unsigned int cardSigmaN = 0;
    std::vector<std::string> labels = getLabels(str, cardSigma, cardSigmaN);

    cout << "CardSigma: " << cardSigma << endl;
    cout << "CardSigmaN: " << cardSigmaN << endl;

    // Converte la stringa in una stringa di interi
    std::map<std::string, unsigned int> labelToIntMap = labelToInt(labels);

    // Stampa la mappa
    std::cout << "Label to int map: " << std::endl;
    for (const auto &pair : labelToIntMap)
    {
        std::cout << pair.first << " -> " << pair.second << std::endl;
    }

    std::string convertedStr = "";
    for (char ch : str)
    {
        if (ch == '(' or ch == ')')
        {
            convertedStr += ch;
        }
        else
        {
            convertedStr += std::to_string(labelToIntMap[std::string(1, ch)]);
        }
    }

    // Stampa la stringa convertita
    std::cout << "Converted string: " << convertedStr << std::endl;

    // Crea l'albero etichettato utilizzando la stringa convertita
    LabeledTree<unsigned int> tree(convertedStr);
    cout << "Tree: " << tree.toString() << endl;

    // build an XBWT
    vector<unsigned int> intNodesPosSorted{};
    XBWT xbwt(tree, cardSigma, cardSigmaN, false, true, &intNodesPosSorted);
    string errors = "";

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
        cout << "Get upward path of " << i << ": " << upwardPath << endl;

        for (unsigned int j = 0; j < upwardPath.size(); ++j)
        {
            auto subPathSearchResult = xbwt.subPathSearch(upwardPath.substr(j));
            cout << "\tSubpath search of " << upwardPath.substr(j) << ": " << subPathSearchResult.first << " " << subPathSearchResult.second << endl;

            for (unsigned int k = subPathSearchResult.first; k <= subPathSearchResult.second; ++k)
            {
                auto checkUpwardPath = xbwt.getUpwardPath(k);
                if (checkUpwardPath.find(upwardPath.substr(j)) == std::string::npos)
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