#ifndef LABELEDTREE_HPP
#define LABELEDTREE_HPP

#include <vector>
#include <optional>
#include <memory>
#include <string>
#include <stack>
#include <sstream>
#include <iostream>

template <typename LabelType>
class Node
{
private:
    LabelType label;
    std::vector<Node *> children;
    Node *parent;

public:
    Node(LabelType lbl, Node *prnt = nullptr, std::vector<Node *> children = {}) : label(lbl), parent(prnt), children(children) {}

    ~Node()
    {        
        for (auto child : children)
            delete child;
    }

    bool isRoot() const
    {
        return parent == nullptr;
    }

    bool isLeaf() const
    {
        return children.empty();
    }

    unsigned int getLevel() const
    {
        unsigned int level = 0;
        Node *current = parent;
        while (current)
        {
            level++;
            current = current->parent;
        }
        return level;
    }

    LabelType getLabel() const
    {
        return label;
    }

    const std::vector<Node *> &getChildren() const
    {
        return children;
    }

    std::optional<Node *> getParent() const
    {
        return parent ? std::optional<Node *>(parent) : std::nullopt;
    }

    bool isRightmost() const
    {
        if (parent)
        {
            const auto &siblings = parent->children;
            return siblings.back() == this;
        }
        return false;
    }

    void pushBackChild(LabelType lbl)
    {
        auto child = new Node(lbl, this);
        children.push_back(child);
    }

    void pushBackChild(Node *child)
    {
        child->parent = this;
        children.push_back(child);
    }

    void prependChild(Node *child)
    {
        child->parent = this;
        children.insert(children.begin(), child);
    }
};

template <typename LabelType>
class LabeledTree
{
private:
    Node<LabelType> *root;

public:
    LabeledTree() : root(nullptr) {}

    LabeledTree(LabelType rootLabel)
    {
        root = new Node<LabelType>(rootLabel);
    }

    LabeledTree(const std::string &str)
    {
        root = fromString(str);
    }

    LabeledTree(Node<LabelType> *root) : root(root) {}

    LabeledTree(const LabeledTree &other)
    {
        // recursively copy the tree
        root = copyTree(other.root);
    }

    ~LabeledTree()
    {
        delete root;
    }

    LabeledTree &operator=(const LabeledTree &other)
    {
        if (this == &other)
        {
            return *this;
        }

        delete root;
        root = copyTree(other.root);

        return *this;
    }

    Node<LabelType> *getRoot() const
    {
        return root;
    }

    void setRoot(Node<LabelType> *newRoot)
    {
        root = newRoot;
    }

    std::vector<Node<LabelType> *> getNodes() const
    {
        std::vector<Node<LabelType> *> nodes;
        collectNodes(root, nodes);
        return nodes;
    }

    std::string toString() const
    {
        std::ostringstream oss;
        toStringHelper(root, oss);
        return oss.str();
    }

private:
    Node<LabelType> *copyTree(Node<LabelType> *node)
    {
        if (!node)
        {
            return nullptr;
        }
        
        auto newNode = new Node<LabelType>(node->getLabel());
        for (const auto &child : node->getChildren())
        {
            newNode->addChild(copyTree(child));
        }

        return newNode;
    }

    Node<LabelType> *fromString(const std::string &str)
    {
        std::stack<Node<LabelType> *> nodeStack;
        std::istringstream iss(str);
        char ch;
        LabelType label;
        Node<LabelType> *currentNode, *root = nullptr;

        while (iss >> ch)
        {
            if (isdigit(ch))
            {
                iss.putback(ch);
                iss >> label;
                if (nodeStack.empty())
                {
                    currentNode = root = new Node<LabelType>(label);
                    nodeStack.push(currentNode);
                }
                else
                {
                    currentNode = nodeStack.top();
                    currentNode->pushBackChild(label);
                    nodeStack.push(currentNode->getChildren().back());
                }
            }
            else if (ch == '(')
            {
                // Do nothing, just continue
            }
            else if (ch == ')')
            {
                nodeStack.pop();
            }
        }

        return root;
    }

    void collectNodes(Node<LabelType> *node, std::vector<Node<LabelType> *> &nodes) const
    {
        nodes.push_back(node);
        for (const auto &child : node->getChildren())
        {
            collectNodes(child, nodes);
        }
    }

    void toStringHelper(const Node<LabelType> *node, std::ostringstream &oss) const
    {
        if (!node)
            return;

        oss << '(';
        oss << node->getLabel();
        if (!node->isLeaf())
        {
            const auto &children = node->getChildren();
            for (size_t i = 0; i < children.size(); ++i)
            {
                toStringHelper(children[i], oss);
            }
        }
        oss << ')';
    }
};

#endif // LABELEDTREE_HPP