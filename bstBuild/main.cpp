#include <iostream>

class BST
{
public:
    BST() = default;
    ~BST() {delete root;}

    void build(int * array)
    {
        delete root;
        root = buildRec(array, 0, sizeof(array));
    }

    void print(){ printRec("", root, false);}

private:
    struct Node
    {
        Node(int val) : left(nullptr), right(nullptr), value(val){}
        ~Node(){delete left; delete right;}
        Node * left, * right;
        int value;
    };

    Node * buildRec(int * array, int start, int end)
    {
        if(start > end) return nullptr;

        int mid = start + (end - start) / 2;
        Node * current = new Node(array[mid]);

        current->left = buildRec(array, start, mid-1);
        current->right = buildRec(array, mid + 1, end);
        return current;
    }

    // https://stackoverflow.com/questions/36802354/print-binary-tree-in-a-pretty-way-using-c
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
    
    Node * root = nullptr;
};

int main ()
{
	BST bst;
	int sortedArray1 [] = {1,2,3,4,5,6,7,8,9,10};
    int sortedArray2 [] = {10,9,8,7,6,5,4,3,2,1};

    bst.build(sortedArray1);
	bst.print();

    std::cout << "\n\n";

    bst.build(sortedArray2);
    bst.print();

}
