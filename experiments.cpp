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
    bool verbose = false;

    // Stringa di input
    // Read input string from file
    string str;
    ifstream inputFile("/home/dvdxl/Scrivania/AMD_project/XBWT/tree_100.txt");
    if (inputFile.is_open())
    {
        getline(inputFile, str);
        inputFile.close();
    }
    else
    {
        cerr << "Unable to open file";
        return 1;
    }

    if (verbose)
        cout << "Input string: " << str << endl;

    auto start = chrono::high_resolution_clock::now();
    LabeledTree<unsigned int> tree(str, [](const std::string &label) -> unsigned int
                               { return static_cast<unsigned int>(std::stoul(label)); });
    auto end = chrono::high_resolution_clock::now();
    cout << "Tree construction time: "
         << chrono::duration_cast<chrono::microseconds>(end - start).count()
         << " micro s" << endl;

    cout << "Number of nodes: " << tree.getNodes().size() << endl;
    cout << "Depth of the tree: " << tree.getDepth() << endl;

    if (verbose)
        cout << "Tree: " << tree.toString() << endl;

    // build an XBWT
    vector<unsigned int> intNodesPosSorted{};
    vector<unsigned int> testIntNodesPosSorted{};

    // Measure time for XBWT construction
    double total_time = 0.0;
    const int num_iterations = 10;
    for (int i = 0; i < num_iterations; ++i) {
        auto start = chrono::high_resolution_clock::now();
        XBWT<unsigned int> xbwt(tree, true, false);
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        total_time += duration;
    }

    double average_time = total_time / num_iterations;
    cout << "Average construction time: " << average_time << " ms" << endl;


    total_time = 0.0;
    for (int i = 0; i < num_iterations; ++i) {
        auto start = chrono::high_resolution_clock::now();
        XBWT<unsigned int> xbwt(tree, false, false);
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        total_time += duration;
    }

    average_time = total_time / num_iterations;
    cout << "Average construction time (no pathSort): " << average_time << " ms" << endl;


    XBWT<unsigned int> xbwt(tree, true, verbose, &intNodesPosSorted);
    XBWT<unsigned int> xbwt2(tree, false, false, &testIntNodesPosSorted);
    cout << endl
         << "Cardinality of the alphabet: " << xbwt.getCardSigma() << endl;

    cout << "\n______________________________________________________\n"
         << endl;
    cout << "Testing construction of XBWT:" << endl;
    string errors = "";

    if (intNodesPosSorted != testIntNodesPosSorted)
        throw std::runtime_error("IntNodesPosSorted is not correct");

    cout << "Test passed!" << endl;
    cout << "______________________________________________________" << endl;

    return 0;
}