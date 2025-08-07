#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <chrono>

#include <LabeledTree.hpp>
#include <XBWT.hpp>

using namespace std;

int main()
{
    bool verbose = true;

    // Stringa di input
    //string str = "(A(B(D(a))(a)(E(b)))(C(D(c))(b)(D(c)))(B(D(b))))";
    // string str = "(A(D(D(D(E(D(E(c)))(c)))(E(c)))(E(c))))";
    // string str = "(A(B(D(C(C(b))(a)(B(a)))(D(E(c)))(D(B(a))(a)(B(c)))(a))(E(B(D(a))(a)(E(C(C(C(C(b))(a)(B(a)))(D(E(c)))(D(B(a))(a)(B(c)))(b))(a)(B(a)))(D(E(c)))(D(B(a))(a)(B(c)))(b)))(C(D(c))(b)(D(c)))(B(D(b)))(B(D(B(D(a))(a)(E(b)))(C(D(c))(b)(D(c)))(B(D(b)))(a))(a)(E(b)))(C(D(c))(b)(D(c)))(B(D(b)))(b)))(C(D(c))(b)(D(c)))(B(D(b))))";
    // string str = "(A(B(C(B(C(B(C(B(a)(b)(c)))))(b)(c)))))";
    //string str = "(a(b(c(b(c(x)(y)(z))))))";
    //string str = "(r(q)(j(m(p(l(t))))(r(h(c))))(k(g)(f(h(i(u(c))))(q(q(h(f(a(f))(w(s))))(p))(v(y)(r(o(n(g)(t(s(m)(n)(w)))(y(m)(d(y(r(t(f(u(v))))))(m(m(t))(b)(w(g))(u(t(k(z)(o(n))))(f(u)(c(f(f)(z(w(n(w)))))))))(f(c(t)(u))(e(u(i)(q(g))(i(x(s)(j))(m))))(o(z)(x(s)(h(f(g))(q))(u(n(x(e(z(e)))(c(t(e))))(i))(o(a(v(p(y(x(v)))(s(l))(q(t)(d(l(f(u(d)(c)(c)(s(n(k))(k)(h(q)))))))))))(v(d(z)(n))(j(i)(k(e(w(n)(e(l(c(c)(b(v)(u)))(r))))))(i)))))(c(v(w(s(b(t)))(n(w)(n(n(u)(n(g(g(r)))))(m(h(t))(l(y)(d)(c)(u(k)(y)))(g(u))(e(w(x(f(t)(d(d(s)))(e)))))(t(c(m)(e(t)(j(f(s))(q(t)))))(p(u(w(l(c(o(z)))(z(k(q)))))(y(k(r(o(v)))))))(j(m(j(n(f)(l)(s)))))))))(o(j)(k(u)(w)(p))(u(q(d)(q)(j(h(k(n)(c(g(c(t(l(i))(t(t)))))(r)))))))(c(y(e(z))(y(y(j(r)))))(p(c(x(u(i)(y(s)(s)(h(v)))(v(t(u)(k))))))))(v(u))))(r(b(b(z)(o(u)(i(h(o(v(n(i))(n(p)(t(l(l))(f(i)))))(z(d))(a(u(b)(e(f(c))(t(x(c(n)(c)(q))(k(l)))))(u(y(s(b(e)(s(c)(o(o)(i)))))(v(z)(k(q))(x(d)(n(q)(d(i(g(p)(q)(g)(x(r(t(r)))))(p(z(m)(z(s)(r(t)))))(z(g)(z(j)(g(e)))(u(r(q(m(a(a(t(n))))))(o(x(k)(c(s)(x)))(t(m)(m)(o)(t(d(p)(k(b(f)(f)))(d(d(w(p(a)))))))))(h(d(p(c(b(e(x(n(o))))))(r(u)(e(g)(c))(h(e(o(d(m)(y(p)(h)(o)(l(z(j))(z)(n(h)(w)(i(j(h))))))(u(d)(q(n(n(m(x))(x(a(k(p))(i(o(q))(w(u(p(g)))(b(j(u)(b(i)(x(z(f)(r(f(p(f(q)))))))(c))))(d(q)))(n(e(m(p)(a(o(d)))(f))(z)(o(t(z)(j(h(x)(z)(n)))(r(j(g))))))))))))(g(e)(h(u(x(o(e(e(h)))))(m(b)(w(m(j(z)(v))))))))(y(b(b(t)))(i(p(f(h(l))(f(u(n)(i(q(x(v)(b(v)))(m(y(m(q(b)(o))))))))))(d(j(f(b(g)))(k(u)))))))))))))))))(g(x(j(r))))))))))))))))))))))))))))))))))(z(z)(y)(t(j)(n(j(u))(z(c(f))(m(z)(o))(f(x(g(f))(h)(z(l(i(d))))(u(z(z)))(c)))))(m(z)(i(j(z(e(n)(i(w(o)))(o(o(t(m)(e(a(w)(q)(j(v)(p(q)(k(l))))(z(o(f(v(g(l)(a)(i(f(n(p)))))))(b(s(p)))(p(c(k(k)(d)(l(r(s)(n(i(k)(j)(g(x(s(o(s(l(t))))(p(s)(m(h(q)(g)(f(e(d)(r(r(u))(u(f(u))(r(w(r(o(d)))))))(l(z)))(n(h(w(k(e(h(i(d)(h))))(v(l(d)(c))))))))))))))))(w(k(y(q(r(u(e))(p(x(o(w))(f)(b(m(o(h))(v)(l(j)(d)))))))))))))))))))(t(d)(e(k(h(b(z)(q(r(o)(a(x)(p(k(g(h(i)(r(c(p)(i(k))(v(h(i(a)(r(d(u(o))(g(g)(z(k(u)(y)))))(i(l(p)))))(f(v(f(n))(f)(d)(s(r(q(v(n(q))(k(w(j(x(u))(p(t(h(r(p)(g))(w(x(a(g)(u(y)(y(e)(b(c(r(o(r(s(z(j(d)(y(y))))(t(v)))))))))(k(s)(v(l(n(r))(r(i(f(b)(n(o)))))))(d(o(x))(r(e(k)))))))))(n(y)(c(f(j(e(z))(s(s(e)(f(w(h)(g(d(j))(n)(x(b(w)(m(k(e(c)))))))))(e(q(q))))))))))))))))))))))))))))))))))(w(o)(j(v)(y(s)(h(q(d))(u(s(p)(f(y(g)(b(l)))(u(h(l))(x(j)(n(g(g(o))(v))(t(y)(t(a(g)))))))))(c(z(o(q)(k(j)(z(b)))))(b(n(v(p(w))(a(x(p(w(e))))))(s(g)(o)))(j(p)(w(g(t))(w(w)(e(h)(p(t)(i(p)(g(b)(h(a(n(y(m(s(p)))))))(j)(e(d(f)))))))(m(f(r(v(o)(b(d))(u(u(v)(v(s(s)(y))(s(a)(j)(w(h)(g(m(z)(j(b)))(p(n(o))(g(e)(l(b(g)(o(c(h)(w(h)))))(p(o)(n(r)(q(k(s(l(f(j(c(u))))(u(b))))))))))))))))))(w(k(j))(s(a(z))(v(o(n)(u(h(k(v(s))))(h(a))(k(l)(r(n(c(o(k)))))(p))))(l(x)(i)(n(r(x(y(q)(m(x)(t)(r(a(t)(v)(u)(k))))))(u(o(w))(w(u(i(x))))(v)(c(l(k(y)))(d(v))(v(r(n(f))(u(s)(h(b(t)(o(r)))(y(o(x))))))))))))(i(c(f(q(q(r))(f(r(z))))))(s(y)(q(v)(o)(d)))(i(m(w)(h(j(u(t)(j(f(i))))))(k(t(q(y)))))))))))))))))(r(m(s)(g)))))))))))))))))))))))))))";
    string str = "(1(0(0(0)(1))(1))(1(0(0)(1))(1)))";

    if (verbose)
        cout << "Input string: " << str << endl;

    auto start = chrono::high_resolution_clock::now();
    LabeledTree<char> tree(str, [](const string &label) -> char
                           { return label[0]; });
    auto end = chrono::high_resolution_clock::now();
    cout << "Tree construction time: "
         << chrono::duration_cast<chrono::microseconds>(end - start).count()
         << " ms" << endl;

    cout << "Number of nodes: " << tree.get_nodes().size() << endl;

    if (verbose)
        cout << "Tree: " << tree.to_string() << endl;

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
         << "Cardinality of the alphabet: " << xbwt.get_card_sigma() << endl;

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
    auto rebuiltTree = xbwt.rebuild_tree();
    end = chrono::high_resolution_clock::now();
    cout << "Rebuild tree time: "
         << chrono::duration_cast<chrono::microseconds>(end - start).count()
         << " ms" << endl;

    if (verbose)
        cout << "Rebuilded tree: " << rebuiltTree.to_string() << endl;

    if (rebuiltTree.to_string() != tree.to_string())
        errors += "Rebuilded tree is not correct\n";

    cout << endl
         << "Testing navigation methods..." << endl;
    unsigned int treeSize = tree.get_nodes().size();
    vector<unsigned int> leafNodes{};
    for (unsigned int i = 0; i < treeSize; ++i)
    {
        auto children = xbwt.get_children(i);
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
            auto rankedChild = xbwt.get_ranked_child(i, k);
            if (verbose)
                cout << "\tGet ranked child of " << i << " with k = " << k << ": " << rankedChild << endl;

            if (rankedChild != children.first + k - 1)
                errors += "Ranked child is not correct\n";

            auto nodeLabel = xbwt.get_node_label(rankedChild);
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
                auto charRankedChild = xbwt.get_char_ranked_child(i, childrenLabels[k], l + 1);
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
            auto parent = xbwt.get_parent(k);
            if (verbose)
                cout << "\tGet parent of " << k << ": " << parent << endl;
            if (parent != i)
                errors += "Parent is not correct\n";

            if (find(leafNodes.begin(), leafNodes.end(), parent) == leafNodes.end())
            {
                auto [first, last] = xbwt.get_children(parent);
                if (first == -1 || last == -1)
                    errors += "Children are not correct\n";
            }
        }
    }

    cout << endl
         << "Testing subPathSearch method..." << endl;
    for (unsigned int i = 0; i < treeSize; ++i)
    {
        auto upwardPath = xbwt.get_upward_path(i);
        if (verbose)
            cout << "Get upward path of " << i << ": " << string(upwardPath.begin(), upwardPath.end()) << endl;

        for (unsigned int j = 0; j < upwardPath.size(); ++j)
        {
            std::vector<char> path(upwardPath.begin() + j, upwardPath.end());
            auto subPathSearchResult = xbwt.sub_path_search(path);
            if (verbose)
                cout << "\tSubpath search of " << std::string(upwardPath.begin() + j, upwardPath.end()) << ": " << subPathSearchResult.first << " " << subPathSearchResult.second << endl; 

            if (subPathSearchResult.first != -1 && subPathSearchResult.second != -1)
            {
                for (unsigned int k = subPathSearchResult.first; k <= subPathSearchResult.second; ++k)
                {
                    auto checkUpwardPath = xbwt.get_upward_path(k);
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