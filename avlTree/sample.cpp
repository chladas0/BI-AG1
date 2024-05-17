#ifndef __PROGTEST__
#include <cassert>
#include <cstdarg>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <array>
#include <random>
#include <type_traits>

// We use std::set as a reference to check our implementation.
// It is not available in progtest :)
#include <set>

template < typename T >
struct Ref {
  size_t size() const { return _data.size(); }
  const T* find(const T& value) const {
    auto it = _data.find(value);
    if (it == _data.end()) return nullptr;
    return &*it;
  }
  bool insert(const T& value) { return _data.insert(value).second; }
  bool erase(const T& value) { return _data.erase(value); }

  auto begin() const { return _data.begin(); }
  auto end() const { return _data.end(); }

  private:
  std::set<T> _data;
};
#endif

namespace config {
  // Enable to check that the tree is AVL balanced.
  inline constexpr bool CHECK_DEPTH = true;

  // Disable if your implementation does not have parent pointers
  inline constexpr bool PARENT_POINTERS = true;
}

template < typename T >
struct Tree {

    ~Tree()
    {
        delete root;
    }

    size_t size() const {return AVLSize;}

    const T * find(const T& value) const
    {
        Node * current = root;
        while(current)
        {
            if(current->equal(value))
                return &current->key;
            current = value < current->key ? current->left : current->right;
        }
        return nullptr;
    }
    struct Node;

    static int getHeight(Node * node)
    {
        return node ? node->height : 0;
    }

    int getBalance(Node * node)
    {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    bool balancedAsAllThingsShouldBe(int x)
    {
        return x == 0;
    }

    void leftRotate(Node * x)
    {
        Node * y = x->right;

        // y is gonna be the new root
        if (!x->parent)
            root = y;
        // update x parent to have the new child y check if x is left or right child
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;

        // the node being reattached from y left to be x right ( does not have to be there)
        if(y->left)
            y->left->parent = x;

        y->parent = x->parent;
        x->parent = y;

        x->right = y->left;
        y->left = x;

        // Update heights
        x->updateHeight();
        y->updateHeight();
    }

    void rightRotate(Node * x)
    {
        Node * y = x->left;

        if(!x->parent)
            root = y;
        else if(x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;

        // the node being reattached from y right to be x left ( does not have to be there)
        if(y->right)
            y->right->parent = x;

        y->parent = x->parent;
        x->parent = y;

        x->left = y->right;
        y->right = x;

        x->updateHeight();
        y->updateHeight();
    }

    bool insert(T value)
    {
        Node ** insertPos = &root;
        Node * parent = nullptr;

        while(*insertPos)
        {
            parent = *insertPos;
            if((*insertPos)->equal(value))
                return false;
            insertPos = value < (*insertPos)->key ? &(*insertPos)->left : &(*insertPos)->right;
        }

        *insertPos = new Node(value, parent);
        ++AVLSize;

        // balance the tree
        for(Node * node = (*insertPos)->parent; node; node = parent)
        {
            parent = node->parent;
            node->updateHeight();

            int balance = getBalance(node);

            // left heavy and came from L
            if(balance == 2 && value < node->left->key)
                rightRotate(node);

            // right heavy and came from R
            else if (balance == -2 && value > node->right->key)
                leftRotate(node);

            // left heavy and came from R
            else if(balance == 2 && value > node->left->key)
            {
                leftRotate(node->left);
                rightRotate(node);
            }

            // right heavy and came from L
            else if(balance == -2 && value < node->right->key)
            {
                rightRotate(node->right);
                leftRotate(node);
            }

            if(balancedAsAllThingsShouldBe(getBalance(node)))
                break;
        }
        return true;
    }

  bool erase(const T& value)
  {
      Node ** deletePos = &root;

      while (*deletePos)
      {
          if (value == (*deletePos)->key)
          {
              Node * toOverwrite = *deletePos;

              if ((*deletePos)->left && (*deletePos)->right)
              {
                  deletePos = &(*deletePos)->right;
                  while ((*deletePos)->left)
                      deletePos = &((*deletePos)->left);

                  toOverwrite->key = (*deletePos)->key;
              }

              Node * toDelete = *deletePos;

              if(toDelete->left)
                  toDelete->left->parent = toDelete->parent;

              if(toDelete->right)
                toDelete->right->parent = toDelete->parent;

              *deletePos = (*deletePos)->left ? (*deletePos)->left : (*deletePos)->right;

              Node * parent;
              int ySign = -1;

              // balance the tree
              for(Node * node = toDelete->parent; node; node = parent)
              {
                  parent = node->parent;
                  node->updateHeight();

                  int balance = getBalance(node);
                  int balanceRight = getBalance(node->right);
                  int balanceLeft = getBalance(node->left);

                  if(balance == -2)
                      ySign = balanceRight;

                  if(balance == 2)
                      ySign = balanceLeft;

                  // rotation is required
                  if(balance == 2 && balanceLeft >= 0)
                      rightRotate(node);

                  else if(balance == -2 && balanceRight <= 0)
                      leftRotate(node);

                  else if(balance == 2 && balanceLeft < 0)
                  {
                      leftRotate(node->left);
                      rightRotate(node);
                  }

                  else if(balance == -2 && balanceRight > 0)
                  {
                      rightRotate(node->right);
                      leftRotate(node);
                  }

                  if(balance == 1 || balance == -1 || ySign == 0)
                      break;
              }

              toDelete->left = toDelete->right = nullptr;
              delete toDelete;
              AVLSize--;
              return true;
          }
          deletePos = value < (*deletePos)->key ? &(*deletePos)->left : &(*deletePos)->right;
      }
      return false;
  }

  void print(){ printRec("", root, false);}
  // https://stackoverflow.com/questions/36802354/print-binary-tree-in-a-pretty-way-using-c
  void printRec(const std::string& prefix, const Node* node, bool isLeft)
  {
      if( node != nullptr )
      {
          std::cout << prefix;
          std::cout << (isLeft ? "├──" : "└──" );

          // print the value of the node
          std::cout << node->key << std::endl;

          // enter the next tree level - left and right branch
          printRec( prefix + (isLeft ? "│   " : "    "), node->right, true);
          printRec( prefix + (isLeft ? "│   " : "    "), node->left, false);
      }
  }

    struct Node
{
    explicit Node(T value, Node * parent) :
    parent(parent), right(nullptr), left(nullptr),
    key(std::move(value)), height(1){}

    ~Node(){delete left; delete right;}

    void updateHeight()
    {
        height = std::max(getHeight(left), getHeight(right)) + 1;
    }

    Node * parent, * right, * left;
    T key;
    int height = 1;

    bool equal(const T & data)
    {
        return !(key < data) && !(data < key);
    }
};


  struct TesterInterface {
    // using Node = ...
    static const Node *root(const Tree *t) { return t->root; }
    // Parent of root must be nullptr, ignore if config::PARENT_POINTERS == false
    static const Node *parent(const Node *n) { return n->parent; }
    static const Node *right(const Node *n) { return n->right; }
    static const Node *left(const Node *n) { return n->left; }
    static const T& value(const Node *n) { return n->key; }
  };

  int AVLSize = 0;
  Node * root = nullptr;
};


#ifndef __PROGTEST__

struct TestFailed : std::runtime_error {
  using std::runtime_error::runtime_error;
};

std::string fmt(const char *f, ...) {
  va_list args1;
  va_list args2;
  va_start(args1, f);
  va_copy(args2, args1);
  
  std::string buf(vsnprintf(nullptr, 0, f, args1), '\0');
  va_end(args1);
  
  vsnprintf(buf.data(), buf.size() + 1, f, args2);
  va_end(args2);

  return buf;
}

template < typename T >
struct Tester {
  Tester() = default;

  void size() const {
    size_t r = ref.size();
    size_t t = tested.size();
    if (r != t) throw TestFailed(fmt("Size: got %zu but expected %zu.", t, r));
  }

  void find(const T& x) const {
    auto r = ref.find(x);
    auto t = tested.find(x);
    bool found_r = r != nullptr;
    bool found_t = t != nullptr;

    if (found_r != found_t) _throw("Find mismatch", found_r);
    if (found_r && *t != x) throw TestFailed("Find: found different value");
  }

  void insert(const T& x, bool check_tree_ = false) {
    auto succ_r = ref.insert(x);
    auto succ_t = tested.insert(x);
    if (succ_r != succ_t) _throw("Insert mismatch", succ_r);
    size();
    if (check_tree_) check_tree();
  }

  void erase(const T& x, bool check_tree_ = false) {
    bool succ_r = ref.erase(x);
    auto succ_t = tested.erase(x);
    if (succ_r != succ_t) _throw("Erase mismatch", succ_r);
    size();
    if (check_tree_) check_tree();
  }

  struct NodeCheckResult {
    const T* min = nullptr;
    const T* max = nullptr;
    int depth = -1;
    size_t size = 0;
  };

  void check_tree() const {
    using TI = typename Tree<T>::TesterInterface;
    auto ref_it = ref.begin();
    bool check_value_failed = false;
    auto check_value = [&](const T& v) {
      if (check_value_failed) return;
      check_value_failed = (ref_it == ref.end() || *ref_it != v);
      if (!check_value_failed) ++ref_it;
    };

    auto r = check_node(TI::root(&tested), decltype(TI::root(&tested))(nullptr), check_value);
    size_t t_size = tested.size();

    if (t_size != r.size) throw TestFailed(
      fmt("Check tree: size() reports %zu but expected %zu.", t_size, r.size));

    if (check_value_failed) throw TestFailed(
      "Check tree: element mismatch");

    size();
  }

  template < typename Node, typename F >
  NodeCheckResult check_node(const Node* n, const Node* p, F& check_value) const {
    if (!n) return {};
    
    using TI = typename Tree<T>::TesterInterface;
    if constexpr(config::PARENT_POINTERS) {
      if (TI::parent(n) != p) throw TestFailed("Parent mismatch.");
    }

    auto l = check_node(TI::left(n), n, check_value);
    check_value(TI::value(n));
    auto r = check_node(TI::right(n), n, check_value);

    if (l.max && !(*l.max < TI::value(n)))
      throw TestFailed("Max of left subtree is too big.");
    if (r.min && !(TI::value(n) < *r.min))
      throw TestFailed("Min of right subtree is too small.");

    if (config::CHECK_DEPTH && abs(l.depth - r.depth) > 1) throw TestFailed(fmt(
      "Tree is not avl balanced: left depth %i and right depth %i.",
      l.depth, r.depth
    ));

    return {
      l.min ? l.min : &TI::value(n),
      r.max ? r.max : &TI::value(n),
      std::max(l.depth, r.depth) + 1, 1 + l.size + r.size
    };
  }

  static void _throw(const char *msg, bool s) {
    throw TestFailed(fmt("%s: ref %s.", msg, s ? "succeeded" : "failed"));
  }

  Tree<T> tested;
  Ref<T> ref;
};


void test_insert() {
  Tester<int> t;

  for (int i = 0; i < 10; i++) t.insert(i, true);
  for (int i = -10; i < 20; i++) t.find(i);

  for (int i = 0; i < 10; i++) t.insert((1 + i * 7) % 17, true);
  for (int i = -10; i < 20; i++) t.find(i);
}

void test_erase() {
  Tester<int> t;

  for (int i = 0; i < 10; i++) t.insert((1 + i * 7) % 17, true);
  for (int i = -10; i < 20; i++) t.find(i);

  for (int i = 3; i < 22; i += 2) t.erase(i, true);
  for (int i = -10; i < 20; i++) t.find(i);
  
  for (int i = 0; i < 10; i++) t.insert((1 + i * 13) % 17 - 8, true);
  for (int i = -10; i < 20; i++) t.find(i);
  
  for (int i = -4; i < 10; i++) t.erase(i, true);
  for (int i = -10; i < 20; i++) t.find(i);
}

enum RandomTestFlags : unsigned {
  SEQ = 1, NO_ERASE = 2, CHECK_TREE = 4
};

void test_random(size_t size, unsigned flags = 0) {
  Tester<size_t> t;
  std::mt19937 my_rand(24707 + size);

  bool seq = flags & SEQ;
  bool erase = !(flags & NO_ERASE);
  bool check_tree = flags & CHECK_TREE;

  for (size_t i = 0; i < size; i++)
    t.insert(seq ? 2*i : my_rand() % (3*size), check_tree);

  t.check_tree();

  for (size_t i = 0; i < 3*size + 1; i++) t.find(i);

  for (size_t i = 0; i < 30*size; i++) switch (my_rand() % 5) {
    case 1: t.insert(my_rand() % (3*size), check_tree);
      break;
    case 2: if (erase) t.erase(my_rand() % (3*size), check_tree);
      break;
    default:
      t.find(my_rand() % (3*size));
  }

  t.check_tree();
}

int main() {

    Tree<int> t;

    t.insert(13);
    t.insert(5);
    t.insert(1);
    t.insert(10);
    t.insert(18);
    t.insert(17);
    t.insert(15);
    t.insert(16);
    t.print();



//  try {
//    std::cout << "Insert test..." << std::endl;
//    test_insert();
//
//    std::cout << "Erase test..." << std::endl;
//    test_erase();
//
//    std::cout << "Tiny random test..." << std::endl;
//    test_random(20, CHECK_TREE);
//
//    std::cout << "Small random test..." << std::endl;
//    test_random(200, CHECK_TREE);
//
//    std::cout << "Big random test..." << std::endl;
//    test_random(50'000);
//
//    std::cout << "Big sequential test..." << std::endl;
//    test_random(50'000, SEQ);
//
//    std::cout << "All tests passed." << std::endl;
//  } catch (const TestFailed& e) {
//    std::cout << "Test failed: " << e.what() << std::endl;
//  }
}

#endif


