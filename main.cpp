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
    std::vector<std::string> labels;
    std::string label = "";
    bool isChild = false;
    for (char ch : str)
    {
        if (ch == '(' or ch == ')')
        {
            if (label.empty())
                continue;

            // add the label to the vector if it is not already present
            if (std::find(labels.begin(), labels.end(), label) == labels.end())
            {
                labels.push_back(label);
                ++cardSigma;
                
                if (ch == ')' and isChild)
                    isChild = false;
                else 
                    ++cardSigmaN;
            }

            if (ch == '(' and !isChild)
                isChild = true;

            label = "";
        }
        else
        {
            label += ch;
        }
    }

    return labels;
}

int main()
{
    // Stringa di input
    std::string str = "(A(B(D(a))(a)(E(b)))(C(D(c))(b)(D(c)))(B(D(b))))";
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
    XBWT xbwt(tree, cardSigma, cardSigmaN, true);

    // rebuild the tree
    auto rebuiltTree = xbwt.rebuildTree();

    return 0;
}