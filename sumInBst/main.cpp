#include <iostream>

class Node {
public:
    int value;
    int size; // size of subtree with this node as root
    int sum;  // sum of subtree with this node as root
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

bool find(Node * root, int k)
{
    if(root == nullptr)
        return false;

    if(k > root->value)
        return find(root->right, k);

    if(k < root->value)
        return find(root->left, k);

    return true;
}


// Vola se po tom co find vrati ze tam key neni
Node* insert(Node* root, int key) {
    if (root == nullptr)
        return new Node(key);

    root->size++;
    root->sum = root->sum + key;

    if (key < root->value)
        root->left = insert(root->left, key);
    else
        root->right = insert(root->right, key);

    return root;
}

// Soucet k nejmensich prvku ve stromu
int pref(Node* root, int k) {
    if (root == nullptr || k <= 0)
        return 0;

    // Odpovedi je suma leveho podstromu
    if(k == getSize(root->left))
        return getSum(root->left);

    // Odpovedi je current root value + sum leveho podstromu
    if (k == getSize(root->left) + 1)
        return root->value + getSum(root->left);

    // Odpoved se nachazi v levem podstromu
    if (k < getSize(root->left))
        return pref(root->left, k);

    // Sectu sumu leveho podstomu + value current root + zavolam se do prava s upravenym k
    // else (k > getSize(root->left) + 1)
    return root->value + getSum(root->left) + pref(root->right, k - (getSize(root->left) + 1));
}

void printRec(const std::string& prefix, const Node* node, bool isLeft)
{
    if( node != nullptr )
    {
        std::cout << prefix;
        std::cout << (isLeft ? "├──" : "└──" );

        // print the value of the node
        std::cout << node->value << std::endl;

        // enter the next tree level - left and right branch
        printRec( prefix + (isLeft ? "│   " : "    "), node->left, true);
        printRec( prefix + (isLeft ? "│   " : "    "), node->right, false);
    }
}

void print(Node * root){ printRec("", root, false);}


// take 1 arguemtns from cli
int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <k>" << std::endl;
        return 1;
    }

    int k = std::stoi(argv[1]);

    Node* root = nullptr;
    int keys[] = {2, 1, 3, 5, 4, 12, 15};

    for (int key : keys)
        root = insert(root, key);

    int result = pref(root, k);

    print(root);

    std::cout << "Sum of the first " << k << " elements in the BST: " << result << std::endl;
    return 0;
}

