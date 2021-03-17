#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <limits.h>

#include "tree.h"  // defines public functions for list ADT

// prototypes for private functions used in tree.c only

TreeNode *node_find(Tree *T, TreeNode *node, KeyType key);
TreeNode *min_find(TreeNode *node);
TreeNode *max_find(TreeNode *node);
void free_node(TreeNode* node);
int internal_count(TreeNode* node, int i);


/* Allocates a new, empty list
 */
Tree *tree_construct (int BalanceOption){
  Tree *T;
  T = (Tree *)malloc(sizeof(Tree));
  T->Root = NULL;
  T->BalanceOption = BalanceOption;
  T->ItemCount = 0;
  T->RecentComparisonCount = 0;

  return T;
}

void tree_destruct (Tree *T){
  free_node(T->Root);
  free(T);
}

/* Insert the memory block into the tree.
 * The function will return 0 if key is already in the tree, will return 1 if the key was * not in the tree but instead added to the tree
 * If the tree is empty, insert the memory block in the root
 */ 
int tree_insert (Tree *T, KeyType Key, DataType DataPointer){
  T->RecentComparisonCount = -1;

  // If the Tree is empty, insert in root and return
  if(T->ItemCount == 0){
    TreeNode *new_Node = (TreeNode *)malloc(sizeof(TreeNode));
    new_Node->NodeItem = Key;
    new_Node->DataPtr = DataPointer;
    new_Node->Parent = NULL;
    new_Node->LeftLink = NULL;
    new_Node->RightLink = NULL;
    T->Root = new_Node;
    T->ItemCount++;
    return 1;
  }

  // Travel the tree to find the position for the new block
  // If the key is already exist, replace the DataPtr and return 0
  // If not, insert and return 1
  TreeNode *node = node_find(T, T->Root, Key);

  if(node->NodeItem == Key){
    free(node->DataPtr);
    node->DataPtr = DataPointer;
    return 0;
  }else {
    TreeNode *new_Node = (TreeNode *)malloc(sizeof(TreeNode));
    new_Node->NodeItem = Key;
    new_Node->DataPtr = DataPointer;
    new_Node->Parent = NULL;
    new_Node->LeftLink = NULL;
    new_Node->RightLink = NULL;
    if(node->NodeItem > Key){
      node->LeftLink = new_Node;
      new_Node->Parent = node;
    }else if(node->NodeItem < Key){
      node->RightLink = new_Node;
      new_Node->Parent = node;
    }
    T->ItemCount++;
  }

  return 1;
}

// BUG if remove root, fix it!
// Bug! Just switch the data block and key
/* There are some remove patterns:
 *   1. Remove the block has no children
 *   2. Remove the block has one children
 *   3. Remove the block has two children
 */
DataType tree_remove (Tree *T, KeyType Key){
  T->RecentComparisonCount = -1;
  // Find the position of remove node
  TreeNode *tem_Node = node_find(T, T->Root, Key);                        // remove node
  // If the key is not in the tree, return NULL
  if(T->ItemCount == 0){
    return NULL;
  }
  if(tem_Node->NodeItem != Key){
    return NULL;
  }
  
  DataType rec_Data = tem_Node->DataPtr;
  // Pattern 1, no children
  if(tem_Node->LeftLink == NULL && tem_Node->RightLink == NULL){
    if(tem_Node == T->Root){
      T->Root = NULL;
    }
    else if(tem_Node->Parent->LeftLink == tem_Node){
        tem_Node->Parent->LeftLink = NULL;
      }else{
        tem_Node->Parent->RightLink = NULL;
      }
    free(tem_Node);
  }
  // Pattern 2, one children
  else if(tem_Node->LeftLink == NULL || tem_Node->RightLink == NULL){
    // If remove node is root
    if(tem_Node == T->Root){
      if(tem_Node->LeftLink == NULL){
        T->Root = tem_Node->RightLink;
        tem_Node->RightLink->Parent = NULL;
      }else{
        T->Root = tem_Node->LeftLink;
        tem_Node->LeftLink->Parent = NULL;
      }
    }
    // If children node is on the remove node right side
    else if(tem_Node->LeftLink == NULL){
      if(tem_Node->Parent->LeftLink == tem_Node){
        tem_Node->Parent->LeftLink = tem_Node->RightLink;
        tem_Node->RightLink->Parent = tem_Node->Parent;
      }else{
        tem_Node->Parent->RightLink = tem_Node->RightLink;
        tem_Node->RightLink->Parent = tem_Node->Parent;
      }
    }
    // Else children node is on the remove node left side
    else{
      if(tem_Node->Parent->LeftLink == tem_Node){
        tem_Node->Parent->LeftLink = tem_Node->LeftLink;
        tem_Node->LeftLink->Parent = tem_Node->Parent;
      }else{
        tem_Node->Parent->RightLink = tem_Node->LeftLink;
        tem_Node->LeftLink->Parent = tem_Node->Parent;
      }
    }

    // free node
    free(tem_Node);
  }

  // Pattern 3, two children
  else{
    TreeNode *rep_Node = max_find(tem_Node->LeftLink);    //replace node
  
    if(rep_Node == NULL){
      rep_Node = tem_Node->LeftLink;
    } 
    // If replace node has left children
    if(rep_Node->LeftLink != NULL){
      TreeNode *min = min_find(rep_Node);           // Find the highest node of replace node
      min->LeftLink = tem_Node->LeftLink;           // conect it to leftlink of remove node, it will be tem->rep_Left->tem_Left
      tem_Node->LeftLink->Parent = min;
      rep_Node->LeftLink->Parent = tem_Node;
      tem_Node->LeftLink = rep_Node->LeftLink;
    }
    // Switch remove node and replace node
    tem_Node->DataPtr = rep_Node->DataPtr;
    tem_Node->NodeItem = rep_Node->NodeItem;
    if(rep_Node->Parent->LeftLink == rep_Node){
      rep_Node->Parent->LeftLink = NULL;
    }else if(rep_Node->Parent->RightLink == rep_Node){
      rep_Node->Parent->RightLink = NULL;
    }
    free(rep_Node);
  }
  T->ItemCount--;
  return rec_Data;
}

/* Find the tree element with the matching key and return a pointer to the data block that is stored in
 * this node in the tree. If the key is not found in the tree then return NULL
 */
 DataType tree_search (Tree *T, KeyType Key){
   T->RecentComparisonCount = -1;
   TreeNode *rec_Node = node_find(T, T->Root, Key);
   if(rec_Node->NodeItem == Key){
     return rec_Node->DataPtr;
   }else {
     return NULL;
   }
 }
/* This function is used to return the number of keys in the tree
 */
 int tree_count(Tree *T){
   return T->ItemCount;
 }

/* This function is used to return RecentComparisonCount, the number of key comparisons for the most recent    call to tree_insert, tree_remove, or tree_search.
 */
 int tree_stats (Tree *T){
   return T->RecentComparisonCount;
 }
 
/* This function is used to return the internal path length of the tree 
 */ 
 int tree_internal_path_len(Tree *T){
   int count;
   count = internal_count(T->Root, 0);
   return count;
 }

 int internal_count(TreeNode* node, int i){
   if(node->LeftLink == NULL && node->RightLink == NULL){
     return i;
   }
   return i +  (node->LeftLink ? internal_count(node->LeftLink, i+1) : 0) + (node->RightLink ? internal_count(node->RightLink, i+1) : 0);
 }

/* This is a recursion function to find the position base on input key
 * It will return the node qualified for input key
 */
 TreeNode *node_find(Tree *T, TreeNode *node, KeyType key){
   T->RecentComparisonCount+=2;
   // If this node's item is equal to key, return
   if(node->NodeItem == key){
   }
   // If this node's item is greater than key 
   else if(node->NodeItem > key){
     // If this node has no children, return   
     if(node->LeftLink == NULL){ 
     T->RecentComparisonCount++;    
     } else{                               // If not, move forward to its children     
       return node_find(T, node->LeftLink, key);
     }
   }
   // If this node's item is less than key
   else if(node->NodeItem < key){
     // If this node has no children, return
     if(node->RightLink == NULL){
     T->RecentComparisonCount++;
     } else{                              // If not, move forward to its children 
       return node_find(T, node->RightLink, key);
     }
   }

   return node;
 }

/* This function is used to find the minimum starting with input node
 * If starting node has no left children, return NULL
 */
 TreeNode *min_find(TreeNode *node){
   if(node->LeftLink == NULL){
     return NULL;
   }
  TreeNode *min = node->LeftLink;
    while(min->LeftLink != NULL){
      min = min->LeftLink;
    }
  return min;
 }
 /* This function is used to find the maximum starting with input node
 * If starting node has no right children, return NULL
 */
 TreeNode *max_find(TreeNode *node){
   if(node->RightLink == NULL){
     return NULL;
   }
  TreeNode *max = node->RightLink;
  while(max->RightLink != NULL){
    max = max->RightLink;
  }
  return max;
 }

/* This function is used to free node
 */
 void free_node(TreeNode* node){
   if(node == NULL){
     return;
   }
   if(node->LeftLink != NULL){
    free_node(node->LeftLink);
   }
   if(node->RightLink != NULL){
    free_node(node->RightLink);
   }
   free(node->DataPtr);
   free(node);
   return;
 }

 /* debugging functions
  */
  int tree_debug_validate_rec(TreeNode *N, int min, int max, int *count){
   if (N == NULL) return TRUE;
   if (N->NodeItem <= min || N->NodeItem >= max) return FALSE;
   assert(N->DataPtr != NULL);
   *count += 1;
   return tree_debug_validate_rec(N->LeftLink, min, N->NodeItem, count) &&
   tree_debug_validate_rec(N->RightLink, N->NodeItem, max, count);
  }
  void tree_debug_validate(Tree *T){
   int size = 0;
   assert(tree_debug_validate_rec(T->Root, INT_MIN, INT_MAX, &size) == TRUE);
   assert(size == T->ItemCount);
  }
  void ugly_print(TreeNode *N, int level) {
   if (N == NULL) return ;
   ugly_print(N->RightLink, level+1) ;
   for (int i=0; i<level; i++) printf(" "); // 5 spaces
   printf("%5d\n", N->NodeItem); // field width is 5
   ugly_print(N->LeftLink, level+1);
   }
  void tree_debug_print_tree(Tree *T) {
    printf("Tree with %d keys\n", T->ItemCount);
    ugly_print(T->Root, 0);
    printf("\n");
    tree_debug_validate(T);
  }
