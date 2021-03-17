/* tree.h 
 */
enum balanceoptions {AVL, BST, TWOTHREET};
#define TRUE    1
#define FALSE   0
typedef int KeyType;
typedef void *DataType;
typedef struct NodeTag {
  KeyType NodeItem;
  DataType DataPtr;
  struct NodeTag *Parent;
  struct NodeTag *LeftLink;
  struct NodeTag *RightLink;
} TreeNode;
typedef struct TreeTag {
  TreeNode *Root;
  int BalanceOption; // must be a balanceoptions
  int ItemCount;
  int RecentComparisonCount;
} Tree;

/* prototype definitions for functions in tree.c */
void tree_destruct (Tree *T);
Tree *tree_construct (int BalanceOption);
int tree_insert (Tree *T, KeyType Key, DataType DataPointer);
DataType tree_remove (Tree *T, KeyType Key); 
DataType tree_search (Tree *T, KeyType Key); 
int tree_count(Tree *T);
int tree_stats (Tree *T); 
int tree_internal_path_len(Tree *T); 
void tree_debug_print_tree(Tree *T);
void tree_debug_validate(Tree *T);




/* vi:set ts=8 sts=4 sw=4 et: */
