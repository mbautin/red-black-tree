#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <stdbool.h>

typedef unsigned char rbt_color_t;
const rbt_color_t RED = 0;
const rbt_color_t BLACK = 1;

const bool RBT_VALIDATION_ENABLED = false;
const bool RBT_DEBUG_ENABLED = false;

#define RBT_DEBUG(...) do { \
  if (RBT_DEBUG_ENABLED) { \
    fputs("[RBT_DEBUG] ", stdout); \
    printf(__VA_ARGS__ ); \
    fputc('\n', stdout); \
  } \
} while (0)

struct __attribute__((__packed__)) node {
  size_t data_size;
  struct node* left;
  struct node* right;
  struct node* parent;
  rbt_color_t color;
  char data[];
};

// Print some information about the node struct.
void print_node_struct_info() {
  struct node node;
  printf("rbt_node size: %ld\n", sizeof(struct node));
  printf("offset of rbt_node.left: %ld\n",
      ((char*) &node.left) - ((char*) &node)); 
  printf("offset of rbt_node.right: %ld\n",
      ((char*) &node.right) - ((char*) &node)); 
  printf("offset of rbt_node.data: %ld\n",
      ((char*) &node.data) - ((char*) &node)); 
}

struct node* rbt_alloc(size_t data_size, char* data, rbt_color_t color) {
  struct node* node = (struct node*) malloc(sizeof(struct node) + data_size);
  node->data_size = data_size;
  node->color = color;
  node->left = NULL;
  node->right = NULL;
  node->parent = NULL;
  if (data_size >= 0) {
    memcpy(node->data, data, data_size);
  }
  return node;
}

void rbt_print(struct node* root, int indent) {
  for (int i = 0; i < indent; ++i) {
    fputc(' ', stdout);
  }

  if (root->color == RED) {
    fputs("RED", stdout);
  } else if (root->color == BLACK) {
    fputs("BLACK", stdout);
  } else {
    fputs("INVALID_COLOR", stdout);
  }
  printf(" [%ld bytes data]: ", root->data_size);
  size_t bytes_to_print = root->data_size < 128 ? root->data_size : 128;
  for (size_t i = 0; i < bytes_to_print; ++i) {
    unsigned char c = (unsigned char) root->data[i];
    if (c == 0 && i == root->data_size - 1) {
      break;
    }
    if (c >= 32 && c <= 127) {
      fputc(c, stdout);
    } else {
      fputc('?', stdout);
    }
  }
  fputc('\n', stdout);
  
  if (root->left != NULL) {
    rbt_print(root->left, indent + 2);
  }
  if (root->right != NULL) {
    rbt_print(root->right, indent + 2);
  }
}

int rbt_compare_data(size_t n1, char* data1, size_t n2, char* data2) {
  size_t min_size = n1 < n2 ? n1 : n2;
  while (min_size > 0) {
    if ((unsigned char) *data1 < (unsigned char) *data2) {
      return -1;
    } else if ((unsigned char) *data1 > (unsigned char) *data2) {
      return 1;
    }
    data1++;
    data2++;
    min_size--;
  }
  if (n1 < n2) return -1;
  if (n1 > n2) return 1;
  return 0;
}

int rbt_compare_nodes(struct node* n1, struct node* n2) {
  return rbt_compare_data(n1->data_size, n1->data, n2->data_size, n2->data);
}

//struct node* rbt_min_node(struct node* n1, struct node* n2) {
//  if (n1 == NULL) return n2;
//  if (n2 == NULL) return n1;
//  return rbt_compare_nodes(n1, n2) < 0 ? n1 : n2;
//}
//
//struct node* rbt_max_node(struct node* n1, struct node* n2) {
//  if (n1 == NULL) return n2;
//  if (n2 == NULL) return n1;
//  return rbt_compare_nodes(n1, n2) < 0 ? n2 : n1;
//}

void test_suite() {
  assert(rbt_compare_data(3, "abc", 3, "bcd") == -1);
  assert(rbt_compare_data(3, "abc", 2, "ab") == 1);
  assert(rbt_compare_data(2, "ab", 3, "abc") == -1);
  assert(rbt_compare_data(2, "ab", 2, "ab") == 0);
  assert(rbt_compare_data(4, "123\x7f", 4, "123\x80") == -1);
  assert(rbt_compare_data(4, "123\x80", 4, "123\x7f") == 1);
  assert(rbt_compare_data(1, "J", 1, "7") == 1);
  assert(rbt_compare_data(1, "7", 1, "J") == -1);
}

struct node *grandparent(struct node *n) {
  if ((n != NULL) && (n->parent != NULL))
    return n->parent->parent;
  else
    return NULL;
}
 

struct node *uncle(struct node *n) {
  struct node *g = grandparent(n);
  if (g == NULL)
    return NULL; // No grandparent means no uncle
  if (n->parent == g->left)
    return g->right;
  else
    return g->left;
}

void insert_case2(struct node *n, struct node** root_ptr);
void insert_case3(struct node *n, struct node** root_ptr);
void insert_case4(struct node *n, struct node** root_ptr);
void insert_case5(struct node *n, struct node** root_ptr);
  
char* rbt_node_to_str(struct node* n) {
  static char buf[128];
  if (n == NULL) {
    return "NULL";
  }
  snprintf(buf, sizeof(buf) - 1, "Node with data '%s'", n->data);
  return buf;
}

void insert_case1(struct node *n, struct node** root_ptr)
{
  RBT_DEBUG("case 1 for %s", rbt_node_to_str(n));
  if (n->parent == NULL)
    n->color = BLACK;
  else
    insert_case2(n, root_ptr);
}

void insert_case2(struct node *n, struct node** root_ptr)
{
  RBT_DEBUG("case 2 for %s", rbt_node_to_str(n));
  if (n->parent->color == BLACK)
    return; /* Tree is still valid */
  else
    insert_case3(n, root_ptr);
}

void insert_case3(struct node *n, struct node** root_ptr)
{
  RBT_DEBUG("case 3 for %s", rbt_node_to_str(n));
  struct node *u = uncle(n), *g;

  if ((u != NULL) && (u->color == RED)) {
    n->parent->color = BLACK;
    u->color = BLACK;
    g = grandparent(n);
    g->color = RED;
    insert_case1(g, root_ptr);
  } else {
    insert_case4(n, root_ptr);
  }
}

void insert_case4(struct node *n, struct node** root_ptr) {
  RBT_DEBUG("case 4 for %s", rbt_node_to_str(n));
  struct node *g = grandparent(n);

  if ((n == n->parent->right) && (n->parent == g->left)) {
    /*
     * rotate_left can be the below because of already having *g =  grandparent(n) 
     *
     * struct node *saved_p=g->left, *saved_left_n=n->left;
     * g->left=n; 
     * n->left=saved_p;
     * saved_p->right=saved_left_n;
     * 
     * and modify the parent's nodes properly
     */

    struct node *saved_p = g->left, *saved_left_n = n->left;
    g->left = n;
    n->parent = g;
    
    n->left = saved_p;
    saved_p->parent = n;
        
    saved_p->right = saved_left_n;
    if (saved_left_n != NULL) {
      saved_left_n->parent = saved_p;
    }

    n = n->left;
  } else if ((n == n->parent->left) && (n->parent == g->right)) {
    /*
     * rotate_right can be the below to take advantage of already having *g =  grandparent(n) 
     *
     * struct node *saved_p=g->right, *saved_right_n=n->right;
     * g->right=n; 
     * n->right=saved_p;
     * saved_p->left=saved_right_n;
     * 
     */
    struct node *saved_p = g->right, *saved_right_n = n->right;
    g->right = n;
    n->parent = g;
    
    n->right = saved_p;
    saved_p->parent = n;
    
    saved_p->left = saved_right_n;
    if (saved_right_n != NULL) {
      saved_right_n->parent = saved_p;
    }

    n = n->right;
  }
  insert_case5(n, root_ptr);
}

void rotate_right(struct node* g) {
  RBT_DEBUG("rotate_right for %s", rbt_node_to_str(g));
  struct node* old_parent = g->parent;
  struct node* new_root = g->left;
  struct node* lr = new_root->right;
  new_root->right = g;
  g->left = lr;
  if (lr != NULL) {
    lr->parent = g;
  }
  if (g->parent != NULL) {
    if (g == g->parent->left) {
      g->parent->left = new_root;
    } else {
      g->parent->right = new_root;
    }
  }
  g->parent = new_root;
  new_root->parent = old_parent;
}

void rotate_left(struct node* g) {
  RBT_DEBUG("rotate_left for %s", rbt_node_to_str(g));
  struct node* old_parent = g->parent;
  struct node* new_root = g->right;
  struct node* rl = new_root->left;
  new_root->left = g;
  g->right = rl;
  if (rl != NULL) {
    rl->parent = g;
  }
  if (g->parent != NULL) {
    if (g == g->parent->left) {
      g->parent->left = new_root;
    } else {
      g->parent->right = new_root;
    }
  }
  g->parent = new_root;
  new_root->parent = old_parent;
}

void insert_case5(struct node *n, struct node** root_ptr) {
  RBT_DEBUG("case 5 for %s", rbt_node_to_str(n));
  struct node *g = grandparent(n);

  assert(g != NULL);
  n->parent->color = BLACK;
  g->color = RED;
  if (n == n->parent->left) {
    if (g == *root_ptr) {
      *root_ptr = g->left;
    }
    rotate_right(g);
  } else {
    if (g == *root_ptr) {
      *root_ptr = g->right;
    }
    rotate_left(g);
  }
}

// Inserts the given data into the tree. The data is copied into the allocated node, and the caller 
// retains ownership of the passed buffer.
struct node* rbt_insert(struct node* root, size_t data_size, char* data, bool* inserted) {
  if (root == NULL) {
    return rbt_alloc(data_size, data, BLACK);
  }

  struct node* node = root;

  struct node* new_node = rbt_alloc(data_size, data, RED);
  while (node != NULL) {
    int comp_result = rbt_compare_data(data_size, data, node->data_size, node->data);
    if (comp_result == 0) {
      // Already present.
      *inserted = false;
      return root;
    }
    if (comp_result < 0) {
      if (node->left == NULL) {
        node->left = new_node;
        break;
      }
      node = node->left;
    } else {
      if (node->right == NULL) {
        node->right = new_node;
        break;
      }
      node = node->right;
    }
  }
  new_node->parent = node;
  
  insert_case1(new_node, &root);
  *inserted = true;
  return root;
}

void rbt_validate_bst(struct node* node, struct node* min_value, struct node* max_value) {
  RBT_DEBUG("rbt_validate_bst: %s", rbt_node_to_str(node));
  if (node == NULL)
    return;
  if (min_value != NULL) {
    assert(rbt_compare_nodes(min_value, node) < 0);
  }
  if (max_value != NULL) {
    assert(rbt_compare_nodes(node, max_value) < 0);
  }
  
  if (node->left != NULL) {
    assert(node->left->parent == node);
    assert(rbt_compare_nodes(node->left, node) < 0);
    rbt_validate_bst(node->left, min_value, node);
  }
  if (node->right != NULL) {
    assert(node->right->parent == node);
    assert(rbt_compare_nodes(node, node->right) < 0);
    rbt_validate_bst(node->right, node, max_value);
  }
}

// Returns the number of black nodes on the path from the root to leaves (not including leaves
// that are always considered black). Also validates that every red node only has black children.
int rbt_validate_invariants(struct node* node) {
  if (node == NULL)
    return 0;

  if (node->color == RED) {
    assert(node->left == NULL || node->left->color == BLACK);
    assert(node->right == NULL || node->right->color == BLACK);
  }
  
  int n_left_black = rbt_validate_invariants(node->left);
  int n_right_black = rbt_validate_invariants(node->right);
  assert(n_left_black == n_right_black);
  return n_left_black + (node->color == BLACK ? 1 : 0);
}

void rbt_validate_tree(struct node* root) {
  if (root == NULL) return;
  assert(root->color == BLACK);
  rbt_validate_bst(root, NULL, NULL);
  rbt_validate_invariants(root);
}

int rbt_num_nodes(struct node* n) {
  if (n == NULL)
    return 0;
  return 1 + rbt_num_nodes(n->left) + rbt_num_nodes(n->right);
}

void generate_random_string(char* dest, int len) {
  for (int i = 0; i < len; ++i) {
    switch (rand() % 4) {
    case 0:
      dest[i] = 'a' + rand() % 26;
      break;
    case 1:
      dest[i] = 'A' + rand() % 26;
      break;
    case 2:
      dest[i] = '0' + rand() % 10;
      break;
    default:
      dest[i] = '_';
    }
  }
  dest[len] = 0;
}

void print_hline() {
  for (int j = 0; j < 80; ++j) {
    fputc('-', stdout);
  }
  fputc('\n', stdout);
}

void rbt_randomized_test() {
  char buf[128];
  struct node* root = NULL;
  bool inserted;
  int expected_num_nodes = 0;
  for (int i = 0; i < 10000000; ++i) {
    if (RBT_DEBUG_ENABLED) {
      print_hline();
      printf("Iteration %d\n", i + 1);
      print_hline();
      fputc('\n', stdout);
    }
      
    int len = rand() % 20 + 1;
    generate_random_string(buf, len);
    
    RBT_DEBUG("Attempting to insert '%s'\n", buf);

    root = rbt_insert(root, len + 1, buf, &inserted);
    if (inserted) ++expected_num_nodes;
    if (RBT_DEBUG_ENABLED) {
      rbt_print(root, 0);
    }
    if (RBT_VALIDATION_ENABLED) {
      rbt_validate_tree(root);
      assert(rbt_num_nodes(root) == expected_num_nodes);
    }
    RBT_DEBUG("\n");
  }
}

int main(void) {
  test_suite();
  RBT_DEBUG("Starting test");
  rbt_randomized_test();
  puts("Test did not crash");
  
	return EXIT_SUCCESS;
}
