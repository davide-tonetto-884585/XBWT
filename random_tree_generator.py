import networkx as nx

def dfs_parenthesis(tree, node, parent):
    """Perform an iterative DFS to create a balanced parenthesis representation of the tree."""
    stack = [(root, None)]  # Stack to store (node, parent)
    result = ""
    visited = set()

    while stack:
        node, parent = stack.pop()
        if node in visited:
            result += ")"  # Close parenthesis for the current node
            continue

        visited.add(node)
        result += "("  # Open parenthesis for the current node
        result += str(node)

        # Add the node again to the stack to close the parenthesis later
        stack.append((node, parent))

        for neighbor in reversed(list(tree.neighbors(node))):
            if neighbor != parent:  # Avoid revisiting the parent node
                stack.append((neighbor, node))

    return result

# Step 1: Generate a random tree with `n` nodes
n = 500  # Change this to the desired size
tree = nx.random_labeled_rooted_tree(n)

# Step 2: Convert the random tree to a balanced parenthesis string
root = 0  # Start DFS from node 0
balanced_parenthesis = dfs_parenthesis(tree, root, parent=None)

# Step 3: Save the balanced parenthesis string to a file
with open(f"tree_{n}.txt", "w") as file:
    file.write(balanced_parenthesis)
    
# Print the result
print("Balanced Parenthesis Representation of the Tree:")
print(balanced_parenthesis)
