#include "XBWT.hpp"
#include <iostream>
#include <stack>
#include <cmath>
#include <algorithm>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/int_vector.hpp>

struct XBWT::MyImpl
{
    sdsl::rrr_vector<> SLastCompressed;
    sdsl::wt_int<sdsl::rrr_vector<>> SAlphaCompressed;
    sdsl::rrr_vector<> SAlphaBitCompressed;
    sdsl::rrr_vector<> ACompressed;
    unsigned int cardSigma;
    unsigned int cardSigmaN;
};

XBWT::XBWT(const LabeledTree<unsigned int> &tree, unsigned int cardSigma, unsigned int cardSigmaN, bool verbose)
{
    pImpl = std::make_unique<MyImpl>();
    pImpl->cardSigma = cardSigma;
    pImpl->cardSigmaN = cardSigmaN;
    createXBWT(tree, verbose);
}

XBWT::~XBWT() = default;

void XBWT::radixSort(std::vector<std::pair<unsigned int, Triplet<unsigned int, int, int>>> &arr) const
{
    auto countSort = [](std::vector<std::pair<unsigned int, Triplet<unsigned int, int, int>>> &arr, unsigned int exp)
    {
        std::vector<std::pair<unsigned int, Triplet<unsigned int, int, int>>> output(arr.size(), std::pair<unsigned int, Triplet<unsigned int, int, int>>(0, Triplet<unsigned int, int, int>(0, 0, 0)));
        std::vector<unsigned int> count(10, 0);

        for (const auto &pair : arr)
        {
            unsigned int val = std::stoi(pair.second.join(""));
            ++count[(val / exp) % 10];
        }

        for (unsigned int i = 1; i < 10; ++i)
        {
            count[i] += count[i - 1];
        }

        for (int i = arr.size() - 1; i >= 0; --i)
        {
            unsigned int val = std::stoi(arr[i].second.join(""));
            output[count[(val / exp) % 10] - 1] = arr[i];
            --count[(val / exp) % 10];
        }

        for (unsigned int i = 0; i < arr.size(); ++i)
        {
            arr[i] = output[i];
        }
    };

    // TODO: check if this is valid for big integers
    unsigned int max = std::stoi(arr[0].second.join(""));
    for (const auto &el : arr)
    {
        unsigned int val = std::stoi(el.second.join(""));
        if (val > max)
        {
            max = val;
        }
    }

    for (unsigned int exp = 1; max / exp > 0; exp *= 10)
    {
        countSort(arr, exp);
    }
}

std::vector<Triplet<unsigned int, int, int>> XBWT::computeIntNodes(const Node<unsigned int> &root) const
{
    std::vector<Triplet<unsigned int, int, int>> intNodes;

    // stack to visit nodes in reorder traversal
    std::stack<Triplet<const Node<unsigned int> *, int, int>> stack;
    stack.push(Triplet<const Node<unsigned int> *, int, int>(&root, 0, 0));

    while (!stack.empty())
    {
        auto [node, cur_level, parent_index] = stack.top();
        stack.pop();
        intNodes.push_back(Triplet<unsigned int, int, int>(node->getLabel(), cur_level, parent_index));

        // update level and parent index
        parent_index = intNodes.size();
        ++cur_level;

        // visit children in reverse order
        for (auto it = node->getChildren().rbegin(); it != node->getChildren().rend(); ++it)
        {
            stack.push(Triplet<const Node<unsigned int> *, int, int>(*it, cur_level, parent_index));
        }
    }

    return intNodes;
}

Node<unsigned int> *XBWT::contractTree(std::vector<Triplet<unsigned int, int, int>> intNodes, short int j, std::vector<std::pair<unsigned int, Triplet<unsigned int, int, int>>> *tripletsSorted) const
{
    if (tripletsSorted)
    {
        unsigned int newName = 2;
        Triplet<unsigned int, int, int> &preTriplet = tripletsSorted->front().second;
        for (auto &pair : *tripletsSorted)
        {
            if (pair.second.first != preTriplet.first || pair.second.second != preTriplet.second || pair.second.third != preTriplet.third)
            {
                preTriplet = pair.second;
                ++newName;
            }

            intNodes[pair.first].first = newName;
        }
    }

    if (j == 0)
        intNodes[0].first = 1;

    // create nodes
    /*std::vector<Node<unsigned int> *> nodes(intNodes.size(), nullptr);
    for (unsigned int i = 0; i < intNodes.size(); ++i)
    {
        nodes[i] = new Node<unsigned int>(intNodes[i].first);
    }*/

    short int jNext[] = {1, 2, 0};
    std::vector<std::pair<unsigned int, unsigned int>> edges;
    if (j == 0)
    {
        for (int i = intNodes.size() - 1; i >= 0; --i)
        {
            auto &it = intNodes[i];
            if (it.second % 3 != j)
            {
                if (it.second % 3 == jNext[j])
                {
                    // if the parent is the root
                    if (it.third == 1)
                        edges.push_back(std::pair<unsigned int, unsigned int>(it.third - 1, i));
                    else
                        edges.push_back(std::pair<unsigned int, unsigned int>(intNodes[it.third - 1].third - 1, i));
                }
                else
                    edges.push_back(std::pair<unsigned int, unsigned int>(it.third - 1, i));
            }
        }
    }
    else
    {
        for (int i = intNodes.size() - 1; i > 0; --i)
        {
            auto &it = intNodes[i];
            if (it.second % 3 != j)
            {
                if (it.second % 3 == jNext[j])
                    edges.push_back(std::pair<unsigned int, unsigned int>(intNodes[it.third - 1].third - 1, i));
                else
                    edges.push_back(std::pair<unsigned int, unsigned int>(it.third - 1, i));
            }
        }
    }

    // create tree by adding edges in reverse order
    std::vector<Node<unsigned int> *> nodes(intNodes.size(), nullptr);
    for (int i = edges.size() - 1; i >= 0; --i)
    {
        if (nodes[edges[i].first] == nullptr)
            nodes[edges[i].first] = new Node<unsigned int>(intNodes[edges[i].first].first);

        if (nodes[edges[i].second] == nullptr)
            nodes[edges[i].second] = new Node<unsigned int>(intNodes[edges[i].second].first);

        nodes[edges[i].first]->addChild(nodes[edges[i].second]);
    }

    // auto tree = LabeledTree<unsigned int>(nodes[0]);
    // std::cout << "Tree: " << tree.toString() << std::endl;

    return nodes[0];
}

std::vector<unsigned int> XBWT::pathSortMerge(std::vector<unsigned int> &intNodesPosNotJSorted, std::vector<unsigned int> &firstIndexIntNodesPosNotJSorted, std::vector<bool> &indexFoundIntNodesPosNotJSorted, std::vector<unsigned int> &intNodesPosJSorted, std::vector<Triplet<unsigned int, int, int>> &tempIntNodes, unsigned int numDummyNodes, short int jv, bool dummyRoot, bool firstIt) const
{
    std::vector<unsigned int> merged(intNodesPosNotJSorted.size() + intNodesPosJSorted.size(), 0);
    short int jCond[] = {1, 2, 0};
    bool flag = true;
    unsigned int i = 0, j = 0, cont = 0; // i for not j, j for j
    while (flag)
    {
        if (tempIntNodes[intNodesPosNotJSorted[i] + numDummyNodes].second % 3 == jCond[jv])
        {
            std::pair<unsigned int, unsigned int> pair1{
                tempIntNodes[tempIntNodes[intNodesPosNotJSorted[i] + numDummyNodes].third].first,
                tempIntNodes[tempIntNodes[tempIntNodes[intNodesPosNotJSorted[i] + numDummyNodes].third].third].first};

            std::pair<unsigned int, unsigned int> pair2{
                tempIntNodes[tempIntNodes[intNodesPosJSorted[j] + numDummyNodes].third].first,
                tempIntNodes[tempIntNodes[tempIntNodes[intNodesPosJSorted[j] + numDummyNodes].third].third].first};

            if (!firstIt)
            {
                pair1.first = tempIntNodes[intNodesPosNotJSorted[i] + numDummyNodes].first;
                pair2.first = tempIntNodes[intNodesPosJSorted[j] + numDummyNodes].first;
            }

            if (pair1.first == pair2.first)
            {
                if (!firstIt || pair1.second == pair2.second)
                {
                    /* long int index1 = -1, index2 = -1;
                    for (unsigned int k = 0; k < intNodesPosNotJSorted.size(); ++k)
                    {
                        if (index1 == -1 && intNodesPosNotJSorted[k] == tempIntNodes[intNodesPosJSorted[j] + numDummyNodes].third - numDummyNodes)
                            index1 = k;

                        if (index2 == -1 && intNodesPosNotJSorted[k] == intNodesPosNotJSorted[i])
                            index2 = k;

                        if (index1 != -1 && index2 != -1)
                            break;
                    } */

                    if (!indexFoundIntNodesPosNotJSorted[tempIntNodes[intNodesPosJSorted[j] + numDummyNodes].third - numDummyNodes] ||
                        !indexFoundIntNodesPosNotJSorted[intNodesPosNotJSorted[i]])
                        throw std::runtime_error("Error: index not found in merge");

                    unsigned int index1 = firstIndexIntNodesPosNotJSorted[tempIntNodes[intNodesPosJSorted[j] + numDummyNodes].third - numDummyNodes];
                    unsigned int index2 = firstIndexIntNodesPosNotJSorted[intNodesPosNotJSorted[i]];

                    if (index1 > index2)
                        merged[cont++] = intNodesPosNotJSorted[i++];
                    else
                        merged[cont++] = intNodesPosJSorted[j++];
                }
                else if (pair1.second < pair2.second)
                    merged[cont++] = intNodesPosNotJSorted[i++];
                else
                    merged[cont++] = intNodesPosJSorted[j++];
            }
            else if (pair1.first < pair2.first)
                merged[cont++] = intNodesPosNotJSorted[i++];
            else
                merged[cont++] = intNodesPosJSorted[j++];
        }
        else if (tempIntNodes[intNodesPosNotJSorted[i] + numDummyNodes].second % 3 == jCond[jCond[jv]])
        {
            unsigned int v1 = tempIntNodes[tempIntNodes[intNodesPosNotJSorted[i] + numDummyNodes].third].first;
            unsigned int v2 = tempIntNodes[tempIntNodes[intNodesPosJSorted[j] + numDummyNodes].third].first;

            if (!firstIt)
            {
                v1 = tempIntNodes[intNodesPosNotJSorted[i] + numDummyNodes].first;
                v2 = tempIntNodes[intNodesPosJSorted[j] + numDummyNodes].first;
            }

            if (v1 == v2)
            {
                /* long int index1 = -1, index2 = -1;
                for (unsigned int k = 0; k < intNodesPosJSorted.size(); ++k)
                {
                    if (index1 == -1 && intNodesPosNotJSorted[k] == tempIntNodes[intNodesPosJSorted[j] + numDummyNodes].third - numDummyNodes)
                        index1 = k;

                    if (index2 == -1 && intNodesPosNotJSorted[k] == tempIntNodes[intNodesPosNotJSorted[i] + numDummyNodes].third - numDummyNodes)
                        index2 = k;

                    if (index1 != -1 && index2 != -1)
                        break;
                } */

                if (!indexFoundIntNodesPosNotJSorted[tempIntNodes[intNodesPosJSorted[j] + numDummyNodes].third - numDummyNodes] ||
                    !indexFoundIntNodesPosNotJSorted[tempIntNodes[intNodesPosNotJSorted[i] + numDummyNodes].third - numDummyNodes])
                    throw std::runtime_error("Error: index not found in merge");

                unsigned int index1 = firstIndexIntNodesPosNotJSorted[tempIntNodes[intNodesPosJSorted[j] + numDummyNodes].third - numDummyNodes];
                unsigned int index2 = firstIndexIntNodesPosNotJSorted[tempIntNodes[intNodesPosNotJSorted[i] + numDummyNodes].third - numDummyNodes];

                if (index1 > index2)
                    merged[cont++] = intNodesPosNotJSorted[i++];
                else
                    merged[cont++] = intNodesPosJSorted[j++];
            }
            else if (v1 < v2)
                merged[cont++] = intNodesPosNotJSorted[i++];
            else
                merged[cont++] = intNodesPosJSorted[j++];
        }

        if (j >= intNodesPosJSorted.size())
        {
            for (unsigned int k = i; k < intNodesPosNotJSorted.size(); ++k)
                merged[cont++] = intNodesPosNotJSorted[k];

            flag = false;
        }

        if (i >= intNodesPosNotJSorted.size())
        {
            for (unsigned int k = j; k < intNodesPosJSorted.size(); ++k)
                merged[cont++] = intNodesPosJSorted[k];

            flag = false;
        }
    }

    if (dummyRoot && !firstIt)
    {
        // remove dummy root
        merged.erase(merged.begin());
    }

    return merged;
}

std::vector<unsigned int> XBWT::pathSort(const LabeledTree<unsigned int> &cTree, std::vector<Triplet<unsigned int, int, int>> *intNodes, bool dummyRoot, bool firstIt, unsigned int rem) const
{
    std::vector<Triplet<unsigned int, int, int>> localIntNodes;
    if (intNodes == nullptr)
    {
        localIntNodes = computeIntNodes(*cTree.getRoot());
        intNodes = &localIntNodes;
    }
    else
    {
        *intNodes = computeIntNodes(*cTree.getRoot());
    }

    // get number of nodes at level j I {0, 1, 2} mode 3
    long int nodeLevelCounts[] = {0, 0, 0};
    for (const auto &triplet : *intNodes)
    {
        ++nodeLevelCounts[triplet.second % 3];
    }

    nodeLevelCounts[0] -= rem;

    // t/3
    long int x = 0;
    if (rem > 0)
        x = static_cast<unsigned int>(std::ceil(static_cast<double>((*intNodes).size() - rem) / 3.0));
    else
        x = static_cast<unsigned int>(std::ceil(static_cast<double>((*intNodes).size()) / 3.0));

    // compute j value
    short int j = -1;
    for (unsigned int i = 0; i < 3; ++i)
    {
        if (nodeLevelCounts[i] >= x)
        {
            j = i;
            break;
        }
    }

    // TODO: remove this
    if (j == -1)
    {
        throw std::runtime_error("Error: j value not found");
    }

    std::vector<unsigned int> intNodesPosJ(nodeLevelCounts[j], 0);
    std::vector<unsigned int> intNodesPosNotJ((*intNodes).size() - nodeLevelCounts[j], 0);
    for (unsigned int i = 0, k = 0, l = 0; i < (*intNodes).size(); ++i)
    {
        if ((*intNodes)[i].second % 3 == j)
        {
            intNodesPosJ[k++] = i;
        }
        else
        {
            intNodesPosNotJ[l++] = i;
        }
    }

    // prepemd intNodes with dummy root if needed (2 or 3 based on j level)
    short int numDummyNodes = (j != 0) ? 3 : 2;
    std::vector<Triplet<unsigned int, int, int>> tempIntNodes((*intNodes).size() + numDummyNodes, Triplet<unsigned int, int, int>(0, 0, 0));
    if (j != 0)
    {
        tempIntNodes[0] = Triplet<unsigned int, int, int>(0, -3, -1);
        tempIntNodes[1] = Triplet<unsigned int, int, int>(0, -2, 0);
        tempIntNodes[2] = Triplet<unsigned int, int, int>(0, -1, 1);

        for (unsigned int i = 0; i < (*intNodes).size(); ++i)
        {
            tempIntNodes[i + numDummyNodes] = (*intNodes)[i];
            tempIntNodes[i + numDummyNodes].third += numDummyNodes - 1;
        }
    }
    else
    {
        tempIntNodes[0] = Triplet<unsigned int, int, int>(0, -2, -1);
        tempIntNodes[1] = Triplet<unsigned int, int, int>(0, -1, 0);

        for (unsigned int i = 0; i < (*intNodes).size(); ++i)
        {
            tempIntNodes[i + numDummyNodes] = (*intNodes)[i];
            tempIntNodes[i + numDummyNodes].third += numDummyNodes - 1;
        }
    }

    std::vector<std::pair<unsigned int, Triplet<unsigned int, int, int>>> tripletsToSort(intNodesPosNotJ.size(), std::pair<unsigned int, Triplet<unsigned int, int, int>>(0, Triplet<unsigned int, int, int>(0, 0, 0)));
    bool areAllEqual = true;
    for (unsigned int i = 0; i < intNodesPosNotJ.size(); ++i)
    {
        auto index = intNodesPosNotJ[i] + numDummyNodes;
        tripletsToSort[i].first = intNodesPosNotJ[i];

        tripletsToSort[i].second.first = tempIntNodes[tempIntNodes[index].third].first;
        index = tempIntNodes[index].third;

        tripletsToSort[i].second.second = tempIntNodes[tempIntNodes[index].third].first;
        index = tempIntNodes[index].third;

        tripletsToSort[i].second.third = tempIntNodes[tempIntNodes[index].third].first;

        if (areAllEqual && (tripletsToSort[i].second.first != tripletsToSort[0].second.first || tripletsToSort[i].second.second != tripletsToSort[0].second.second || tripletsToSort[i].second.third != tripletsToSort[0].second.third))
        {
            areAllEqual = false;
        }
    }

    radixSort(tripletsToSort);

    std::vector<unsigned int> intNodesPosNotJSorted(intNodesPosNotJ.size(), 0);
    if (!areAllEqual)
    {
        LabeledTree<unsigned int> cTree;
        if (firstIt)
            cTree.setRoot(contractTree((*intNodes), j, &tripletsToSort));
        else
            cTree.setRoot(contractTree((*intNodes), j));

        ++rem;
        if (j == 0)
            intNodesPosNotJSorted = pathSort(cTree, nullptr, true, false, rem);
        else
            intNodesPosNotJSorted = pathSort(cTree, nullptr, false, false, rem);
    }
    else
    {
        for (unsigned int i = 0; i < intNodesPosNotJSorted.size(); ++i)
            intNodesPosNotJSorted[i] = intNodesPosNotJ[tripletsToSort[i].first];
    }

    if (j == 0)
    {
        for (auto &el : intNodesPosNotJSorted)
            --el;
    }

    std::vector<unsigned int> firstIndex(intNodes->size(), 0);
    std::vector<bool> indexFound(intNodes->size(), false);
    for (unsigned int i = 0; i < intNodesPosNotJSorted.size(); ++i)
    {
        intNodesPosNotJSorted[i] = intNodesPosNotJ[intNodesPosNotJSorted[i]];

        if (!indexFound[intNodesPosNotJSorted[i]])
        {
            firstIndex[intNodesPosNotJSorted[i]] = i;
            indexFound[intNodesPosNotJSorted[i]] = true;
        }
    }

    std::vector<Triplet<unsigned int, unsigned int, unsigned int>> pairIntNodesPosJSorted(intNodesPosJ.size(), Triplet<unsigned int, unsigned int, unsigned int>(0, 0, 0));
    short int c = 0;
    if (!firstIt)
    {
        if (j == 0)
        {
            pairIntNodesPosJSorted[0].first = tempIntNodes[intNodesPosJ[0] + numDummyNodes].first;
            pairIntNodesPosJSorted[0].second = 0;
            pairIntNodesPosJSorted[0].third = intNodesPosJ[0];
            c = 1;
        }

        for (unsigned int i = c; i < intNodesPosJ.size(); ++i)
        {
            /*long int index = -1;
            for (unsigned int k = 0; k < intNodesPosNotJSorted.size(); ++k)
            {
                if (intNodesPosNotJSorted[k] == tempIntNodes[intNodesPosJ[i] + numDummyNodes].third - numDummyNodes)
                {
                    index = k;
                    break;
                }
            }*/

            if (!indexFound[tempIntNodes[intNodesPosJ[i] + numDummyNodes].third - numDummyNodes])
                throw std::runtime_error("Error: index not found");

            pairIntNodesPosJSorted[i].first = tempIntNodes[intNodesPosJ[i] + numDummyNodes].first;
            pairIntNodesPosJSorted[i].second = firstIndex[tempIntNodes[intNodesPosJ[i] + numDummyNodes].third - numDummyNodes];
            pairIntNodesPosJSorted[i].third = intNodesPosJ[i];
        }
    }
    else
    {
        if (j == 0)
        {
            pairIntNodesPosJSorted[0].first = tempIntNodes[tempIntNodes[intNodesPosJ[0] + numDummyNodes].third].first;
            pairIntNodesPosJSorted[0].second = 0;
            pairIntNodesPosJSorted[0].third = intNodesPosJ[0];
            c = 1;
        }

        for (unsigned int i = c; i < intNodesPosJ.size(); ++i)
        {
            /* long int index = -1;
            for (unsigned int k = 0; k < intNodesPosNotJSorted.size(); ++k)
            {
                if (intNodesPosNotJSorted[k] == tempIntNodes[intNodesPosJ[i] + numDummyNodes].third - numDummyNodes)
                {
                    index = k;
                    break;
                }
            } */

            if (!indexFound[tempIntNodes[intNodesPosJ[i] + numDummyNodes].third - numDummyNodes])
                throw std::runtime_error("Error: index not found");

            pairIntNodesPosJSorted[i].first = tempIntNodes[tempIntNodes[intNodesPosJ[i] + numDummyNodes].third].first;
            pairIntNodesPosJSorted[i].second = firstIndex[tempIntNodes[intNodesPosJ[i] + numDummyNodes].third - numDummyNodes];
            pairIntNodesPosJSorted[i].third = intNodesPosJ[i];
        }
    }

    // sort by first and second element
    std::sort(pairIntNodesPosJSorted.begin(), pairIntNodesPosJSorted.end(), [](const Triplet<unsigned int, unsigned int, unsigned int> &a, const Triplet<unsigned int, unsigned int, unsigned int> &b)
              { return a.first < b.first || (a.first == b.first && a.second < b.second); });

    std::vector<unsigned int> intNodesPosJSorted(intNodesPosJ.size(), 0);
    for (unsigned int i = 0; i < pairIntNodesPosJSorted.size(); ++i)
    {
        intNodesPosJSorted[i] = pairIntNodesPosJSorted[i].third;
    }

    return pathSortMerge(intNodesPosNotJSorted, firstIndex, indexFound, intNodesPosJSorted, tempIntNodes, numDummyNodes, j, dummyRoot, firstIt);
}

void XBWT::createXBWT(const LabeledTree<unsigned int> &tree, bool verbose)
{
    std::vector<Triplet<unsigned int, int, int>> intNodes;
    auto posIntNodesSorted = pathSort(tree, &intNodes);

    std::vector<unsigned int> posLast(intNodes.size(), 0);
    std::vector<bool> _SLast(intNodes.size(), false);
    std::vector<bool> _SAlphaBit(intNodes.size(), true);
    for (unsigned int i = 1; i < intNodes.size(); ++i)
    {
        posLast[intNodes[i].third - 1] = i;
        _SAlphaBit[intNodes[i].third - 1] = false;
    }

    for (unsigned int i : posLast)
    {
        if (i != 0)
            _SLast[i] = true;
    }

    sdsl::bit_vector SLast(intNodes.size());
    sdsl::int_vector<> tempSAlpha(intNodes.size());
    sdsl::bit_vector SAlphaBit(intNodes.size()); // SAlphaBit[i] = 1 iff the corresponding label is a leaf label
    sdsl::bit_vector A(intNodes.size()); // A[1] = 1, A[j] = 1 iff the first symbol of Sπ[j] differs from the first symbol of Sπ[j − 1]
    unsigned int cont = 0;
    unsigned int prev_label = 0;
    for (unsigned int i : posIntNodesSorted)
    {
        SLast[cont] = _SLast[i];
        tempSAlpha[cont] = intNodes[i].first;
        SAlphaBit[cont] = _SAlphaBit[i];
        A[cont] = (cont == 0) ? 1 : (intNodes[intNodes[i].third - 1].first != prev_label);
        prev_label = intNodes[intNodes[i].third - 1].first;
        ++cont;
    }

    // Compress the bit vectors
    pImpl->SLastCompressed = sdsl::rrr_vector<>(SLast);
    pImpl->SAlphaBitCompressed = sdsl::rrr_vector<>(SAlphaBit);
    pImpl->ACompressed = sdsl::rrr_vector<>(A);

    // Create the wavelet tree
    sdsl::construct_im(pImpl->SAlphaCompressed, tempSAlpha);

    if (verbose)
    {
        // print
        std::cout << "(SLast, SAlpha, SAlphaBit, A) " << std::endl;
        for (unsigned int i = 0; i < intNodes.size(); ++i)
        {
            std::cout << "(" << (pImpl->SLastCompressed[i] ? "1" : "0") << ", " << pImpl->SAlphaCompressed[i] << ", " << (pImpl->SAlphaBitCompressed[i] ? "1" : "0") << ", " << pImpl->ACompressed[i] << ") " << std::endl;
        }

        std::cout << "SLast size: " << sdsl::size_in_bytes(SLast) << " B" << std::endl;
        std::cout << "SLast compressed size: " << sdsl::size_in_bytes(pImpl->SLastCompressed) << " B" << std::endl;
        std::cout << "SAlpha size: " << sdsl::size_in_bytes(tempSAlpha) << " B" << std::endl;
        std::cout << "SAlpha compressed size: " << sdsl::size_in_bytes(pImpl->SAlphaCompressed) << " B" << std::endl;
        std::cout << "SAlphaBit size: " << sdsl::size_in_bytes(SAlphaBit) << " B" << std::endl;
        std::cout << "SAlphaBit compressed size: " << sdsl::size_in_bytes(pImpl->SAlphaBitCompressed) << " B" << std::endl;
        std::cout << "A size: " << sdsl::size_in_bytes(A) << " B" << std::endl;
        std::cout << "A compressed size: " << sdsl::size_in_bytes(pImpl->ACompressed) << " B" << std::endl;
    }
}

std::vector<unsigned int> XBWT::buildF() const
{
    std::vector<unsigned int> C(pImpl->cardSigma, 0);
    for (unsigned int i = 0; i < pImpl->SLastCompressed.size(); ++i)
        ++C[pImpl->SAlphaCompressed[i] - 1];

    std::vector<unsigned int> F(pImpl->cardSigmaN, 0);
    F[0] = 1;
    // iter internal labels cardinality
    for (unsigned int i = 0; i < pImpl->cardSigmaN; ++i)
    {
        unsigned int s = 0, j = F[i];
        while (s != C[i])
        {
            if (pImpl->SLastCompressed[j++] == 1)
                ++s;
        }
        
        F[i + 1] = j;
    }

    return F;
}

LabeledTree<unsigned int> XBWT::rebuildTree() const
{
    auto F = buildF();
    for (unsigned int i = 0; i < F.size(); ++i)
    {
        std::cout << "F[" << i << "] = " << F[i] << std::endl;
    }

    return LabeledTree<unsigned int>();
}
