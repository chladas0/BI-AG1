#include <iostream>
using namespace std;

class Node {
public:
    int key;
    Node* left, * right;
    Node(int key) : key(key), left(nullptr), right(nullptr) {}
};

class TreePair
{
public:
    TreePair(Node * t1, Node * t2) : T1(t1), T2(t2){}
    Node * T1;
    Node * T2;
};

Node* insert(Node* root, int key) {

    if (root == nullptr)
        return new Node(key);

    if (key < root->key)
        root->left = insert(root->left, key);
    else
        root->right = insert(root->right, key);

    return root;
}

// https://stackoverflow.com/questions/36802354/print-binary-tree-in-a-pretty-way-using-c
void printRec(const std::string& prefix, const Node* node, bool isLeft)
{
    if( node != nullptr )
    {
        std::cout << prefix;
        std::cout << (isLeft ? "├──" : "└──" );

        // print the key of the node
        std::cout << node->key << std::endl;

        // enter the next tree level - left and right branch
        printRec( prefix + (isLeft ? "│   " : "    "), node->left, true);
        printRec( prefix + (isLeft ? "│   " : "    "), node->right, false);
    }
}
void print(Node * root){ printRec("", root, false);}


TreePair bstSplit(Node * v, int s)
{
    Node * T1, * T2;
    if(v == nullptr)
        return {nullptr, nullptr};

    // v with the left subtree will be the root of the T1, we split rightSubtree
    // and set v.right to the T1 returned from splitting
    // T2 will be determined in the right subtree recursion
    if(v->key <= s)
    {
        TreePair T = bstSplit(v->right, s);
        v->right = T.T1;
        T1 = v; T2 = T.T2;
    }

    // v with the right subtree will be the root of the T2, we split leftSubtree
    // and set v.left to the T2 returned from splitting
    // T1 will be determined in the left subtree recursion
    else if (v->key > s)
    {
        TreePair T = bstSplit(v->left, s);
        v->left = T.T2;
        T2 = v; T1 = T.T1;
    }

    return {T1, T2};
}

Node * bstBuild(int * array, int st, int en)
{
    if(st > en)
        return nullptr;

    int mid = st + (en - st ) / 2;

    Node * cur = new Node(array[mid]);

    cur->left = bstBuild(array, st, mid - 1);
    cur->right = bstBuild(array, mid + 1, en);

    return cur;
}

int main() {
    int keys[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

    Node  *root = bstBuild(keys, 0, size(keys) - 1);

    print(root);

    TreePair result = bstSplit(root, 4);

    print(result.T1);
    cout << "\n\n\n";
    print(result.T2);
    return 0;
}

