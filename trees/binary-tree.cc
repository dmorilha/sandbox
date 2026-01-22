#include <iostream>

#include <cassert>

template<class TYPE>
struct Node {
  using CLASS = Node<TYPE>;
  CLASS
    * left = nullptr,
    * parent = nullptr,
    * right = nullptr;

  TYPE value;

  std::size_t height = 0;

  template<class ... ARGS>
  Node(ARGS && ... args) : value(std::forward<ARGS>(args)...) { }

  bool operator < (CLASS & other) const {
    return value < other.value;
  }
};

template<class TYPE>
class BinaryTree {
public:
  // methods should appear lexicographically ordered
  void insert(TYPE && type) {
    std::cout << __func__ << " " << type << std::endl;
    insert(root_, new NODE(std::forward<TYPE>(type)));
    std::cout << __func__ << std::endl << std::endl;
  }

  void insert(const TYPE type) {
    std::cout << __func__ << " " << type << std::endl;
    insert(root_, new NODE(type));
    std::cout << __func__ << std::endl << std::endl;
  }

  template<class F>
  void traverse_in_order(F && f) {
    std::cout << __func__ << std::endl;
    traverse_in_order(root_, f);
  }

  template<class F>
  void traverse_breath_first(F && f) {
    std::cout << __func__ << std::endl;
    traverse_breath_first(root_, f);
  }

  template<class F>
  TYPE * find(const F & f) {
    return find(f, root_);
  }

private:
  using NODE = Node<TYPE>;

  void insert(NODE * & current, NODE * node, NODE * parent = nullptr) {
    if (nullptr != current) {
      if (*node < *current) {
        /* all nodes in the left sub-tree compare lesser than the current node. */
        insert(current->left, node, current);
      } else {
        /* all nodes in the right sub-tree compare greater than or equal to the current node. */
        insert(current->right, node, current);
      }
    } else {
      current = node;
      current->parent = parent;
      if (nullptr != parent) {
        update_height(parent, 1);
      }
    }
  }

  /*
   * the whole tree's balance predicates into all leafs being at most 1 level apart.
   * therefore when a parent has unbalanced children, it has to be rotated.
   * the child with more levels assumes its position, and it takes place as one of the children.
   */
  void update_height(NODE * const node, const std::size_t new_height) {
    std::cout << "start " << __func__ << " " << node->value << " " << node->height << std::endl;

    bool balanced = false;

    assert(new_height >= node->height);
    if (new_height == node->height) {
      return;
    }

    /* TODO: check if this condition only happens once per update_height recursion */
    if (1 < new_height) {
      if (nullptr == node->left || node->left->height + 2 < new_height) {
        NODE * const tmp = node->right;

        node->right = tmp->left;

        if (nullptr != node->right) {
          node->right->parent = node;
          node->height = 1 + node->right->height;
        } else {
          node->height = nullptr != node->left ? 1 + node->left->height : 0;
        }

        tmp->left = node;
        tmp->parent = node->parent;
        if (nullptr == node->parent) {
          root_ = tmp;
        } else {
          if (node->parent->right == node) {
            node->parent->right = tmp;
          } else {
            node->parent->left = tmp;
          }
        }
        node->parent = tmp;

        balanced = true;
        std::cerr << "balanced to the right" << std::endl;
      } else if (nullptr == node->right || node->right->height + 2 < new_height) {
        NODE * const tmp = node->left;

        node->left = tmp->right;

        if (nullptr != node->left) {
          node->left->parent = node;
          node->height = 1 + node->left->height;
        } else {
          node->height = nullptr != node->right ? 1 + node->right->height : 0;
        }

        tmp->right = node;
        tmp->parent = node->parent;
        if (nullptr == node->parent) {
          root_ = tmp;
        } else {
          if (node->parent->right == node) {
            node->parent->right = tmp;
          } else {
            node->parent->left = tmp;
          }
        }
        node->parent = tmp;

        balanced = true;
        std::cerr << "balanced to the left" << std::endl;
      }
    }

    if ( ! balanced) {
      node->height = new_height;
    }

    std::cout << "end " << __func__ << " " << node->value << " " << node->height << std::endl;

    if (nullptr != node->parent) {
      update_height(node->parent, node->height + 1);
    } else /* the only node which does not have a parent is the root of the tree */ {
      root_ = node;
    }
  }

  template<class F>
  void traverse_in_order(NODE * & current, const F & f) {
    if (nullptr != current->left) {
      traverse_in_order(current->left, f);
    }
    f(*current);
    if (nullptr != current->right) {
      traverse_in_order(current->right, f);
    }
  }

  template<class F>
  void traverse_breath_first(NODE * & current, const F & f) {
    const bool left = nullptr != current->left,
          right = nullptr != current->right;

    f(*current);

    if ( ! (left | right)) {
      std::cerr << "node is leaf" << std::endl;
    } else {
      if (left) {
        traverse_breath_first(current->left, f);
      } else {
        std::cerr << "current->left is null" << std::endl;
      }

      if (right) {
        traverse_breath_first(current->right, f);
      } else {
        std::cerr << "current->right is null" << std::endl;
      }
    }
  }

  template<class F>
  TYPE * find(const F & f, NODE * const node) {
    if (nullptr == node) {
      return nullptr;
    }
    const int result = f(node->value);
    if (0 > result) {
      return find(f, node->left);
    } else if (0 < result) {
      return find(f, node->right);
    } else {
      return &(node->value);
    }
  }

  NODE * root_ = nullptr;
};

int main() {
  BinaryTree<int> tree;
  for (int i = 1; i < 16; ++i) { tree.insert(i); }
  tree.traverse_in_order([](const auto & node){ std::cout << node.value << std::endl; });
  std::cout << std::endl;
  tree.traverse_breath_first([](const auto & node){ std::cout << node.value << std::endl; });
  std::cout << std::endl;
  std::cout << "result for 13 -> " << tree.find([](const int value){ return 13 - value; }) << std::endl;
  return 0;
}
