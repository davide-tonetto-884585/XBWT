#include "XBWT.hpp"
#include <iostream>
#include <stack>
#include <cmath>

struct XBWT::MyImpl {
    // TODO: use sdsl to store the XBWT
};

XBWT::XBWT(const LabeledTree<unsigned int> &tree) {
    createXBWT(tree);
}

/**
 * @brief 
 * IntNodes
        Array of triples, where the first element represents the node 
        label, the second the level, and the third the position of the 
        node's parent in this Array. The order of the nodes is established 
        by the pre-order visit of the T tree..
 * 
 * @param root 
 * @return std::vector<Triplet<unsigned int, int, int>> 
 */
std::vector<Triplet<unsigned int, int, int>> XBWT::computeIntNodes(const Node<unsigned int> &root) {
    std::vector<Triplet<unsigned int, int, int>> intNodes;

    // stack to visit nodes in reorder traversal
    std::stack<Triplet<const Node<unsigned int>*, int, int>> stack;
    stack.push(Triplet<const Node<unsigned int>*, int, int>(&root, 0, 0));

    while (!stack.empty()) {
        auto [node, cur_level, parent_index] = stack.top();
        stack.pop();
        intNodes.push_back(Triplet<unsigned int, int, int>(node->getLabel(), cur_level, parent_index));
        
        // update level and parent index
        parent_index = intNodes.size() - 1;
        ++cur_level;

        // visit children in reverse order
        for (auto it = node->getChildren().rbegin(); it != node->getChildren().rend(); ++it) {
            stack.push(Triplet<const Node<unsigned int>*, int, int>(*it, cur_level, parent_index));
        }
    }

    return intNodes;
}

void XBWT::pathSort(LabeledTree<unsigned int> &cTree, bool dummyRoot, bool firstIt, unsigned int rem, unsigned int maxName) {
    auto intNodes = computeIntNodes(*cTree.getRoot());

    // get number of nodes at level j I {0, 1, 2} mode 3
    Triplet<unsigned int, unsigned int, unsigned int> nodeLevelCounts = {0, 0, 0};
    for (const auto &triplet : intNodes) {
        for (unsigned int i = 0; i < 3; ++i) {
            ++nodeLevelCounts[triplet.second % 3];
        }
    }

    // t/3
    unsigned int x = 0;
    if (rem > 0) 
        x = static_cast<unsigned int>(std::ceil(static_cast<double>(intNodes.size() - rem) / 3.0));
    else
        x = static_cast<unsigned int>(std::ceil(static_cast<double>(intNodes.size()) / 3.0));

    // compute j value
    unsigned int j = -1;
    for (unsigned int i = 0; i < 3; ++i) {
        if (nodeLevelCounts[i] >= x) {
            j = i;
            break;
        }
    }

    // TODO: remove this
    if (j == -1) {
        std::cerr << "Error: j value not found" << std::endl;
        return;
    }

    std::vector<unsigned int> intNodesPosJ(nodeLevelCounts[j], 0);
    std::vector<unsigned int> intNodesPosNotJ(intNodes.size() - nodeLevelCounts[j], 0);
    for (unsigned int i = 0, k = 0, l = 0; i < intNodes.size(); ++i) {
        if (intNodes[i].second % 3 == j) {
            intNodesPosJ[k++] = i;
        } else {
            intNodesPosNotJ[l++] = i;
        }
    }

    
}

void XBWT::createXBWT(const LabeledTree<unsigned int> &tree) {
    std::vector<Triplet<unsigned int, int, int>> intNodes = computeIntNodes(*tree.getRoot());
    for (const auto &triplet : intNodes) {
        std::cout << triplet.first << " " << triplet.second << " " << triplet.third << std::endl;
    }
}
