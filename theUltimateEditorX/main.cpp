#ifndef __PROGTEST__
#include <cassert>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>
#include <type_traits>

#endif

template < typename T >
struct Array {

    ~Array() {delete root;}

    struct Node;

    static size_t getHeight(Node * node)
    {
        return node ? node->height : 0;
    }

    static size_t getSize(Node * node)
    {
        return node ? node->size : 0;
    }

    static size_t getNewLineCount(Node * node)
    {
        return node ? node->newlineCount : 0;
    }

    int getBalance(Node * node) const
    {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    T & getAtIndex(size_t index) const
    {
        Node * current = root;

        while(current)
        {
            size_t leftSize = getSize(current->left);

            if(index < leftSize)
                current = current->left;
            else if(index > leftSize)
            {
                index = index - leftSize - 1;
                current = current->right;
            }
            else
                return current->value;
        }
        throw std::out_of_range("Index out of range");
    }

    T& operator [] (size_t index)
    {
        return index < root->size ? getAtIndex(index) : throw std::out_of_range("Index out of range");
    }

    const T& operator [] (size_t index) const
    {
        return index < root->size ? getAtIndex(index) : throw std::out_of_range("Index out of range");
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

        // Update heights and sizes
        x->update();
        y->update();
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

        // Update heights and sizes
        x->update();
        y->update();
    }

    void insert(size_t index, T value)
    {
        if(index > getSize(root))
            throw std::out_of_range("Index out of range");

        Node ** insertPos = &root;
        Node * parent = nullptr;

        while(*insertPos)
        {
            parent = *insertPos;

            size_t leftSize = getSize((*insertPos)->left);

            (*insertPos)->size++;
            if(value == '\n')
                (*insertPos)->newlineCount++;

            if(index <= leftSize)
                insertPos = &(*insertPos)->left;
            else
            {
                index = index - leftSize - 1;
                insertPos = &(*insertPos)->right;
            }
        }

        *insertPos = new Node(value, parent);
        (*insertPos)->update();

        // balance the tree
        for(Node * node = (*insertPos)->parent; node; node = parent)
        {
            parent = node->parent;
            node->update();

            int balance = getBalance(node);

            // left heavy and came from L
            if(balance == 2 && getBalance(node->left) == 1)
                rightRotate(node);

                // right heavy and came from R
            else if (balance == -2 && getBalance(node->right) == -1)
                leftRotate(node);

                // left heavy and came from R
            else if(balance == 2 && getBalance(node->left) == -1)
            {
                leftRotate(node->left);
                rightRotate(node);
            }

                // right heavy and came from L
            else if(balance == -2 && getBalance(node->right) == 1)
            {
                rightRotate(node->right);
                leftRotate(node);
            }

            if(getBalance(node) == 0)
                break;
        }
    }


    T erase(size_t index)
    {
        if(index >= getSize(root))
            throw std::out_of_range("Index out of range");

        Node ** deletePos = &root;

        while (*deletePos)
        {
            size_t leftSize = getSize((*deletePos)->left);
            (*deletePos)->size--;

            if(index < leftSize)
                deletePos = &(*deletePos)->left;
            else if (index > leftSize)
            {
                index = index - leftSize - 1;
                deletePos = &(*deletePos)->right;
            }
            else
            {
                T toRet = (*deletePos)->value;
                Node * toOverwrite = *deletePos;

                if ((*deletePos)->left && (*deletePos)->right)
                {
                    deletePos = &(*deletePos)->right;
                    (*deletePos)->size--;

                    while ((*deletePos)->left)
                    {
                        deletePos = &(*deletePos)->left;
                        (*deletePos)->size--;
                    }

                    toOverwrite->value = (*deletePos)->value;
                }

                Node * toDelete = *deletePos;

                // update the parent of toDelete child
                if(toDelete->left)
                    toDelete->left->parent = toDelete->parent;

                if(toDelete->right)
                    toDelete->right->parent = toDelete->parent;

                *deletePos = (*deletePos)->left ? (*deletePos)->left : (*deletePos)->right;

                // balance the tree
                for(Node * node = toDelete->parent, * parent; node; node = parent)
                {
                    parent = node->parent;
                    node->update();

                    int balance = getBalance(node);
                    int balanceRight = getBalance(node->right);
                    int balanceLeft = getBalance(node->left);

                    // right heavy and came from R
                    if(balance == 2 && balanceLeft >= 0)
                        rightRotate(node);

                        // left heavy and came from L
                    else if(balance == -2 && balanceRight <= 0)
                        leftRotate(node);

                        // left heavy and came from R
                    else if(balance == 2 && balanceLeft == -1)
                    {
                        leftRotate(node->left);
                        rightRotate(node);
                    }

                        // right heavy and came from L
                    else if(balance == -2 && balanceRight == 1)
                    {
                        rightRotate(node->right);
                        leftRotate(node);
                    }
                }

                toDelete->left = toDelete->right = nullptr;
                delete toDelete;
                return toRet;
            }
        }
        throw std::out_of_range("Index out of range");
    }

    // Needed to test the structure of the tree.
    // Replace Node with the real type of your nodes
    // and implementations with the ones matching
    // your attributes.

    bool empty() const{ return getSize(root) == 0;}

    size_t size() const{ return getSize(root);}

    struct Node
    {
        explicit Node(T value, Node * parent) : parent(parent), right(nullptr), left(nullptr),
                                                value(std::move(value)), height(1), size(1){}

        ~Node(){delete left; delete right;}

        void update()
        {
            newlineCount = getNewLineCount(left) + getNewLineCount(right) + ((value == '\n') ? 1 : 0);
            height = std::max(getHeight(left), getHeight(right)) + 1;
            size = getSize(left) + getSize(right) + 1;
        }

        Node * parent, * right, * left;
        T value;
        size_t height = 1, size = 1, newlineCount = 0;
    };
    Node * root = nullptr;
};


typedef Array<char>::Node Node;

struct TextEditorBackend {
  TextEditorBackend(const std::string& text)
  {
      for(auto c : text)
          insert(data.size(), c);
  }

  size_t size() const
  {
      return data.size();
  }

  size_t lines() const
  {
      return data.root ? Array<char>::getNewLineCount(data.root) + 1 : 1;
  }

  char at(size_t i) const
  {
      return data[i];
  }

  void edit(size_t i, char c)
  {
      Node * current = data.root;

      bool wasNewLine = data[i] == '\n';

      bool decrease = wasNewLine && c != '\n';
      bool increase = !wasNewLine && c == '\n';

      while(current)
      {
          size_t leftSize = Array<char>::getSize(current->left);

          // decrease or increase the newlineCount as we go down to the index
          if(decrease)
              current->newlineCount--;
          else if(increase)
              current->newlineCount++;

          if(i < leftSize)
              current = current->left;
          else if(i > leftSize)
          {
              i = i - leftSize - 1;
              current = current->right;
          }
          else
          {
              current->value = c;
              break;
          }
      }
  }

  void insert(size_t i, char c)
  {
      data.insert(i, c);
  }

  void erase(size_t i)
  {
      data.erase(i);
  }

  size_t line_start(size_t r) const
  {
      if(r >= lines())
          throw std::out_of_range("Row out of range");

      size_t index = 0;
      Node * current = data.root;

      while(current)
      {
          size_t leftNewLines = Array<char>::getNewLineCount(current->left);

          if(r <= leftNewLines )
              current = current->left;

          else if (r > leftNewLines)
          {
              // we skipped all the indexes in the left and the index of current node
              index += Array<char>::getSize(current->left) + 1;
              r = r - leftNewLines - (current->value == '\n');
              current = current->right;
          }
      }
      return index;
  }

  size_t line_length(size_t r) const
  {
      if(r >= lines())
          throw std::out_of_range("Index out of range");
      // we can not access start of r+1-th line
      if(r == lines() - 1)
            return data.size() - line_start(r);

      return line_start(r+1) - line_start(r);
  }

  size_t char_to_line(size_t i) const
  {
      if(i >= size())
          throw std::out_of_range("Index out of range");

      size_t r = 0;
      Node * current = data.root;

      while(current)
      {
          size_t leftSize = Array<char>::getSize(current->left);

          if(i < leftSize)
              current = current->left;
          else if (i > leftSize)
          {
              r += Array<char>::getNewLineCount(current->left) + (current->value == '\n');
              i = i - leftSize - 1;
              current = current->right;
          }
          else
              return r + Array<char>::getNewLineCount(current->left);
      }
      return 42;
  }
  Array<char> data;
};

#ifndef __PROGTEST__

////////////////// Dark magic, ignore ////////////////////////

template < typename T >
auto quote(const T& t) { return t; }

std::string quote(const std::string& s) {
  std::string ret = "\"";
  for (char c : s) if (c != '\n') ret += c; else ret += "\\n";
  return ret + "\"";
}

#define STR_(a) #a
#define STR(a) STR_(a)

#define CHECK_(a, b, a_str, b_str) do { \
    auto _a = (a); \
    decltype(a) _b = (b); \
    if (_a != _b) { \
      std::cout << "Line " << __LINE__ << ": Assertion " \
        << a_str << " == " << b_str << " failed!" \
        << " (lhs: " << quote(_a) << ")" << std::endl; \
      fail++; \
    } else ok++; \
  } while (0)

#define CHECK(a, b) CHECK_(a, b, #a, #b)

#define CHECK_ALL(expr, ...) do { \
    std::array _arr = { __VA_ARGS__ }; \
    for (size_t _i = 0; _i < _arr.size(); _i++) \
      CHECK_((expr)(_i), _arr[_i], STR(expr) "(" << _i << ")", _arr[_i]); \
  } while (0)

#define CHECK_EX(expr, ex) do { \
    try { \
      (expr); \
      fail++; \
      std::cout << "Line " << __LINE__ << ": Expected " STR(expr) \
        " to throw " #ex " but no exception was raised." << std::endl; \
    } catch (const ex&) { ok++; \
    } catch (...) { \
      fail++; \
      std::cout << "Line " << __LINE__ << ": Expected " STR(expr) \
        " to throw " #ex " but got different exception." << std::endl; \
    } \
  } while (0)

////////////////// End of dark magic ////////////////////////


std::string text(const TextEditorBackend& t) {
  std::string ret;
  for (size_t i = 0; i < t.size(); i++) ret.push_back(t.at(i));
  return ret;
}

void test1(int& ok, int& fail) {
	TextEditorBackend s("123\n456\n789");
  CHECK(s.size(), 11);
	CHECK(text(s), "123\n456\n789");
  CHECK(s.lines(), 3);
  CHECK_ALL(s.char_to_line, 0,0,0,0, 1,1,1,1, 2,2,2);
  CHECK_ALL(s.line_start, 0, 4, 8);
  CHECK_ALL(s.line_length, 4, 4, 3);
}

void test2(int& ok, int& fail) {
	TextEditorBackend t("123\n456\n789\n");
  CHECK(t.size(), 12);
	CHECK(text(t), "123\n456\n789\n");
  CHECK(t.lines(), 4);
  CHECK_ALL(t.char_to_line, 0,0,0,0, 1,1,1,1, 2,2,2,2);
  CHECK_ALL(t.line_start, 0, 4, 8, 12);
  CHECK_ALL(t.line_length, 4, 4, 4, 0);
}

void test3(int& ok, int& fail) {
	TextEditorBackend t("asdfasdfasdf");

  CHECK(t.size(), 12);
  CHECK(text(t), "asdfasdfasdf");
	CHECK(t.lines(), 1);
  CHECK_ALL(t.char_to_line, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	CHECK(t.line_start(0), 0);
	CHECK(t.line_length(0), 12);

	t.insert(0, '\n');
  CHECK(t.size(), 13);
  CHECK(text(t), "\nasdfasdfasdf");
	CHECK(t.lines(), 2);
  CHECK_ALL(t.line_start, 0, 1);
//
  t.insert(4, '\n');
  CHECK(t.size(), 14);
  CHECK(text(t), "\nasd\nfasdfasdf");
	CHECK(t.lines(), 3);
  CHECK_ALL(t.line_start, 0, 1, 5);
//
  t.insert(t.size(), '\n');
  CHECK(t.size(), 15);
  CHECK(text(t), "\nasd\nfasdfasdf\n");
	CHECK(t.lines(), 4);
  CHECK_ALL(t.line_start, 0, 1, 5, 15);
//
	t.edit(t.size() - 1, 'H');
  CHECK(t.size(), 15);
  CHECK(text(t), "\nasd\nfasdfasdfH");
	CHECK(t.lines(), 3);
  CHECK_ALL(t.line_start, 0, 1, 5);
//
	t.erase(8);
  CHECK(t.size(), 14);
  CHECK(text(t), "\nasd\nfasfasdfH");
	CHECK(t.lines(), 3);
  CHECK_ALL(t.line_start, 0, 1, 5);
//
  t.erase(4);
  CHECK(t.size(), 13);
  CHECK(text(t), "\nasdfasfasdfH");
	CHECK(t.lines(), 2);
  CHECK_ALL(t.line_start, 0, 1);
}

void test_ex(int& ok, int& fail) {
	TextEditorBackend t("123\n456\n789\n");
  CHECK_EX(t.at(12), std::out_of_range);

  CHECK_EX(t.insert(13, 'a'), std::out_of_range);
  CHECK_EX(t.edit(12, 'x'), std::out_of_range);
  CHECK_EX(t.erase(12), std::out_of_range);


  CHECK_EX(t.line_start(4), std::out_of_range);
  CHECK_EX(t.line_start(40), std::out_of_range);
  CHECK_EX(t.line_length(4), std::out_of_range);
  CHECK_EX(t.line_length(6), std::out_of_range);
  CHECK_EX(t.char_to_line(12), std::out_of_range);
  CHECK_EX(t.char_to_line(25), std::out_of_range);
}

int main() {
  int ok = 0, fail = 0;
  if (!fail) test1(ok, fail);
  if (!fail) test2(ok, fail);
  if (!fail) test3(ok, fail);
  if (!fail) test_ex(ok, fail);
  if (!fail) std::cout << "Passed all " << ok << " tests!" << std::endl;
  else std::cout << "Failed " << fail << " of " << (ok + fail) << " tests." << std::endl;
}

#endif


