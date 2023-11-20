#include <iostream>

class Node {
public:
    int value;
    int size; // size of subtree with this node as root
    int sum; // sum of subtree with this node as root
    Node* left, * right;
    Node(int key) : value(key), size(1), sum(value), left(nullptr), right(nullptr) {}
};

int getSize(Node * node)
{
    return node ? node->size : 0;
}

int getSum(Node * node)
{
    return node ? node->sum : 0;
}

Node* insert(Node* root, int key) {
    if (root == nullptr)
        return new Node(key);

    root->size++; // we inserted Node under this one so we increase size of the subtree
    root->sum = root->value + getSum(root->left) + getSum(root->right); // update the sum

    if (key < root->value)
        root->left = insert(root->left, key);
    else
        root->right = insert(root->right, key);

    return root;
}


int pref(Node* root, int k) {
    if (root == nullptr || k < 0)
        return 0;

    if (k < getSize(root->left))
        return pref(root->left, k);
    
    if (k == getSize(root->left))
        return root->value + getSum(root->left);

    return getSum(root->left) + root->value + pref(root->right, k - (getSize(root->left) + 1));
}

int main() {
    Node* root = nullptr;
    int keys[] = {2, 1, 3, 5, 4};

    for (int key : keys)
        root = insert(root, key);

    int k = 4;
    int result = pref(root, k);
    std::cout << "Sum of the first " << k + 1 << " elements in the BST: " << result << std::endl;
    return 0;
}

