import networkx as nx
import random
import string

def dfs_parenthesis(tree, node, parent, node_labels):
    """Perform an iterative DFS to create a balanced parenthesis representation of the tree."""
    stack = [(node, None)]  # Stack to store (node, parent)
    result = ""
    visited = set()

    while stack:
        current_node, current_parent = stack.pop()
        if current_node in visited:
            result += ")"  # Close parenthesis for the current node
            continue

        visited.add(current_node)
        result += "("  # Open parenthesis for the current node
        result += node_labels[current_node]  # Use the random label instead of node number

        # Add the node again to the stack to close the parenthesis later
        stack.append((current_node, current_parent))

        for neighbor in reversed(list(tree.neighbors(current_node))):
            if neighbor != current_parent:  # Avoid revisiting the parent node
                stack.append((neighbor, current_node))

    return result

# Step 1: Generate a random tree with `n` nodes
n = 500000 # Change this to the desired size
tree = nx.random_labeled_rooted_tree(n)

# Step 2: Create random alphabet labels for each node
node_labels = {}
for node in tree.nodes():
    # Randomly choose a number from 1 to 50
    node_labels[node] = str(random.randint(1, 50))
    # node_labels[node] = random.choice(string.ascii_lowercase)

# Step 3: Convert the random tree to a balanced parenthesis string
root = 0  # Start DFS from node 0
balanced_parenthesis = dfs_parenthesis(tree, root, parent=None, node_labels=node_labels)

# Step 4: Save the balanced parenthesis string to a file
with open(f"tree_{n}.txt", "w") as file:
    file.write(balanced_parenthesis)
    
# Print the result
print("Balanced Parenthesis Representation of the Tree:")
print(balanced_parenthesis[:200] + "..." if len(balanced_parenthesis) > 200 else balanced_parenthesis)  # Print first 200 chars for large trees
print(f"\nTree saved to tree_{n}.txt")
print(f"Total nodes: {n}")
print(f"Sample node labels: {dict(list(node_labels.items())[:10])}")
