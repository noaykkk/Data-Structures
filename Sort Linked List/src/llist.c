#include <stdlib.h>
#include <assert.h>
#include<stdio.h>

#include "llist.h"        // defines public functions for list ADT

// definitions for private constants used in llist.c only

#define LLIST_SORTED    999999
#define LLIST_UNSORTED -888888

// prototypes for private functions used in llist.c only 

void llist_debug_validate(llist_t *L);
llist_elem_t *llist_findmax(llist_t *list_ptr, llist_elem_t *m, llist_elem_t *n);
void llist_select_sort(llist_t *list_ptr, llist_elem_t *m, llist_elem_t *n);
void llist_itera_sort(llist_t *list_ptr, llist_elem_t *m, llist_elem_t *n);
llist_t *llist_merge_sort(llist_t *list_ptr);
llist_t *Merge(llist_t* list_ptr, llist_t *L, llist_t *R);
void llist_merge_select(llist_t *list_ptr, llist_t *L, llist_t *R);   
void llist_merge_insert(llist_t *list_ptr, llist_elem_t *tem_elem);        

/* ----- below are the functions  ----- */

/* Obtains a pointer to an element stored in the specified list, at the
 * specified list position
 * 
 * list_ptr: pointer to list-of-interest.  A pointer to an empty list is
 *           obtained from llist_construct.
 *
 * pos_index: position of the element to be accessed.  Index starts at 0 at
 *            head of the list, and incremented by one until the tail is
 *            reached.  Can also specify LLPOSITION_FRONT and LLPOSITION_BACK
 *
 * return value: pointer to element accessed within the specified list.  A
 * value NULL is returned if the pos_index does not correspond to an element in
 * the list.
 */
data_t * llist_access(llist_t *list_ptr, int pos_index)
{
    int count=0;
    llist_elem_t *L;
 

    /* handle special cases.
     *   1.  The list is empty
     *   2.  Asking for the head 
     *   3.  Asking for the tail
     *   4.  specifying a position that is out of range.  This is not defined
     *       to be an error in this function, but instead it is assumed the 
     *       calling function correctly specifies the position index
     */
    if (llist_entries(list_ptr) == 0) {
        return NULL;  // list is empty
    }
    else if (pos_index == LLPOSITION_FRONT || pos_index == 0) {
        return list_ptr->ll_front->data_ptr;
    }
    else if (pos_index == LLPOSITION_BACK || pos_index == llist_entries(list_ptr) - 1) {
        return list_ptr->ll_back->data_ptr;
    }
    else if (pos_index < 0 || pos_index >= llist_entries(list_ptr))
        return NULL;   // does not correspond to position in list

    // loop through the list until find correct position index
    L=list_ptr->ll_front;
    for (count = 0; count < pos_index; count++) {
        L = L->ll_next;
    }
    // already verified that pos_index should be valid so L better not be null
    assert(L != NULL);
    assert(L->data_ptr != NULL);
    return L->data_ptr;
}

/* Allocates a new, empty list 
 *
 * If the comparison function is NULL, then the list is unsorted.
 *
 * Otherwise, the list is initially assumed to be sorted.  Note that if 
 * list_insert is used the list is changed to unsorted.  
 *
 * The field sorted can only take values LLIST_SORTED or LLIST_UNSORTED
 *
 * Use llist_destruct to remove and deallocate all elements on a list 
 * and the header block.
 */
llist_t * llist_construct(int (*fcomp)(const data_t *, const data_t *))
{
    llist_t *L;
    L = (llist_t *) malloc(sizeof(llist_t));
    L->ll_front = NULL;
    L->ll_back = NULL;
    L->ll_entry_count = 0;
    L->compare_fun = fcomp;
    if (fcomp == NULL)
        L->ll_sorted_state = LLIST_UNSORTED;
    else
        L->ll_sorted_state = LLIST_SORTED;

    return L;
}

/* Finds an element in a list and returns a pointer to it.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: element against which other elements in the list are compared.
 * 
 * fcomp: function to test if elem_ptr is a match to an item in the list.
 *        Returns 0 if they match and any value not equal to 0 if they do not match.
 *
 * NOTICE: pos_index is returned and is not an input value!
 *
 * The function returns a pointer to the matching element with lowest index if
 * a match if found.  If a match is not found the return value is NULL.
 *
 * The function also returns the integer position of matching element with the
 *           lowest index.  If a matching element is not found, the position
 *           index that is returned should be -1. 
 *
 * pos_index: used as a return value for the position index of matching element
 *
 */
data_t * llist_elem_find(llist_t *list_ptr, data_t *elem_ptr, int *pos_index,  
        int (*fcomp)(const data_t *, const data_t *))
{
    int count = 0;
    llist_elem_t *L;
    data_t *ret_data;
    L = list_ptr->ll_front;
    
    for(count=0; count < list_ptr->ll_entry_count; count++ ){
        ret_data = L->data_ptr;
        if(fcomp(ret_data, elem_ptr) == 0){
            *pos_index = count;            
            break;
        }
        L = L->ll_next;
    }
    if(count == list_ptr->ll_entry_count){
        *pos_index = -1;   
        return NULL;
    }
    return ret_data;
}

/* Deallocates the contents of the specified list, releasing associated memory
 * resources for other purposes.
 */
void llist_destruct(llist_t *list_ptr)            
{

    int i;
    llist_elem_t *L, *R;
    data_t *tem_data;
    R = list_ptr->ll_front;
    for(i=0; i < list_ptr->ll_entry_count; i++){        //  free data_t 
        tem_data = R->data_ptr;
        free(tem_data);
        R->data_ptr = NULL;
        L = R;
        R = R->ll_next;
        if(R != NULL){
           R->ll_prev = NULL;
        }
        free(L);
    }
    list_ptr->ll_front = NULL;
    list_ptr->ll_back = NULL;
    free(list_ptr);                           // free llist_t   
}

/* Inserts the specified data element into the specified list at the specified
 * position.
 *
 * llist_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: pointer to the element to be inserted into list.
 *
 * pos_index: numeric position index of the element to be inserted into the 
 *            list.  Index starts at 0 at head of the list, and incremented by 
 *            one until the tail is reached.  The index can also be equal
 *            to LLPOSITION_FRONT or LLPOSITION_BACK (these are special negative 
 *            values use to provide a short cut for adding to the head
 *            or tail of the list).
 *
 * If pos_index is greater than the number of elements currently in the list, 
 * the element is simply appended to the end of the list (no additional elements
 * are inserted).
 *
 * Note that use of this function results in the list to be marked as unsorted,
 * even if the element has been inserted in the correct position.  That is, on
 * completion of this subroutine the llist_ptr->ll_sorted_state must be equal 
 * to LLIST_UNSORTED.
 */
void llist_insert(llist_t *list_ptr, data_t *elem_ptr, int pos_index)    
{
    int count = 0;
    llist_elem_t *L, *R;        // insert element left side element and right side element
    assert(list_ptr != NULL);
    llist_elem_t *tem_elem = (llist_elem_t *)malloc(sizeof(llist_elem_t));   // set temporary element pointer
    tem_elem->ll_next = NULL;
    tem_elem->ll_prev = NULL;
    tem_elem->data_ptr = elem_ptr;
    
    if(list_ptr->ll_entry_count == 0){              // if the list is empty
        list_ptr->ll_front = tem_elem;
        list_ptr->ll_back = tem_elem;
        list_ptr->ll_entry_count++;
    }
    
    else if(pos_index == LLPOSITION_FRONT || pos_index == 0){   // insert the element in position 0
        list_ptr->ll_front->ll_prev = tem_elem;
        tem_elem->ll_next = list_ptr->ll_front;
        list_ptr->ll_front = tem_elem;
        list_ptr->ll_entry_count++;
    }
    
    else if(pos_index == LLPOSITION_BACK || pos_index == llist_entries(list_ptr) - 1 || pos_index < 0 || pos_index >= llist_entries(list_ptr)){    //insert the element at the end of elements
        L = list_ptr->ll_front;
        for(count = 0; count < llist_entries(list_ptr) - 1; count++){
            L = L->ll_next;
        }
        L->ll_next = tem_elem;
        tem_elem->ll_prev = L;
        list_ptr->ll_back = tem_elem;
        list_ptr->ll_entry_count++;
    }
    
    else{                                                // insert the element in the sequence  (L)->(tem_elem)->(R) 
        R = list_ptr->ll_front;
        for(count = 0; count < pos_index; count++){
            R = R->ll_next;
        }
        L = R->ll_prev;
        tem_elem->ll_next = R;
        tem_elem->ll_prev = R->ll_prev;
        L->ll_next = tem_elem;
        R->ll_prev = tem_elem;
        list_ptr->ll_entry_count++;
    }
    
    // the last three lines of this function must be the following 
    if (list_ptr->ll_sorted_state == LLIST_SORTED) 
        list_ptr->ll_sorted_state = LLIST_UNSORTED;
    
    
}

/* Inserts the element into the specified sorted list at the proper position,
 * as defined by the compare_fun function pointer found in the header block.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: pointer to the element to be inserted into list.
 *
 * If you use llist_insert_sorted, the list preserves its sorted nature.
 *
 * If you use llist_insert, the list will be considered to be unsorted, even
 * if the element has been inserted in the correct position.
 *
 * If the list is not sorted and you call llist_insert_sorted, this subroutine
 * should generate a system error and the program should immediately stop.
 *
 * The comparison procedure must accept two arguments (A and B) which are both
 * pointers to elements of type data_t.  The comparison procedure returns an
 * integer code which indicates the precedence relationship between the two
 * elements.  The integer code takes on the following values:
 *    1: A should be closer to the list head than B
 *   -1: B should be closer to the list head than A
 *    0: A and B are equal in rank
 *
 * Note: if the element to be inserted is equal in rank to an element already
 *       in the list, the newly inserted element will be placed after all the 
 *       elements of equal rank that are already in the list.
 */
void llist_insert_sorted(llist_t *list_ptr, data_t *elem_ptr)
{
    assert(list_ptr != NULL);
    assert(list_ptr->ll_sorted_state == LLIST_SORTED);

    // insert your code here
    llist_elem_t *L, *R;        // insert element left side element and right side element
    assert(list_ptr != NULL);
    llist_elem_t *tem_elem = (llist_elem_t *)malloc(sizeof(llist_elem_t));   // set temporary element pointer
    int count = 1;
    tem_elem->ll_next = NULL;
    tem_elem->ll_prev = NULL;
    tem_elem->data_ptr = elem_ptr;
    R = list_ptr->ll_front;
    
    // comparison procedure. argument A is the element in the list and argument B is the new insert element
    if( list_ptr->ll_entry_count == 0){
        list_ptr->ll_front = tem_elem;
        list_ptr->ll_back = tem_elem;
    }
    else {
        while(R != NULL){
        if(list_ptr->compare_fun(R->data_ptr,tem_elem->data_ptr) == 0){           // if argument A is equal to B , result 0  
            L = R;
            R = R->ll_next;
            if(R != NULL){
               if(list_ptr->compare_fun(R->data_ptr,tem_elem->data_ptr) == -1){           // if A is the last same element
                   break;
               }
               else{                                                                         // if not
                  continue;
               }
            }
            else{
               break;
            }
        }
        else if(list_ptr->compare_fun(R->data_ptr,tem_elem->data_ptr) == -1){
            L = R->ll_prev;                                                          // if argument A is greater than B , result -1
            break;
        }
        L = R;                                                                      // set L and R to next element in the sequence
        R = R->ll_next;
        count++;
       }
    if(L == NULL){                                                                  // if the tem_elem is the first element in the list
        tem_elem->ll_prev = NULL;
        tem_elem->ll_next = R;
        list_ptr->ll_front = tem_elem;
        R->ll_prev = tem_elem;
    }   
    else if(( (L == list_ptr->ll_front) && (R == NULL))){                          // if the tem_elem is the second element in the list
        if( R == list_ptr->ll_back){                                                  // in this case if R is the last element, insert 
            tem_elem->ll_next = R; 
            tem_elem->ll_prev = L;
            L->ll_next = tem_elem;
            R->ll_prev = tem_elem;
        }
        else{                                                                         // in this case if tem_elem is the last element, insert
            tem_elem->ll_next = NULL;
            tem_elem->ll_prev = L;
            L->ll_next = tem_elem;
            list_ptr->ll_back = tem_elem;
        }
    }   
    else if(L == list_ptr->ll_back){                                                // if there are only one element in the list , insert
        tem_elem->ll_next = NULL;
        tem_elem->ll_prev = L;
        L->ll_next = tem_elem;
        list_ptr->ll_back = tem_elem;
    }
    else{                                                                            // normal case , insert the element in L-> tem_elem -> R sequence
        tem_elem->ll_next = R;
        tem_elem->ll_prev = L;
        L->ll_next = tem_elem;
        R->ll_prev = tem_elem;
    } 
    }

    list_ptr->ll_entry_count++;
    // the last line checks if the new list is correct 
}


/* Removes an element from the specified list, at the specified list position,
 * and returns a pointer to the element.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * pos_index: position of the element to be removed.  Index starts at 0 at
 *            head of the list, and incremented by one until the tail is
 *            reached.  Can also specify LLPOSITION_FRONT and LLPOSITION_BACK
 *
 * Attempting to remove an element at a position index that is not contained in
 * the list will result in no element being removed, and a NULL pointer will be
 * returned.
 */
data_t * llist_remove(llist_t *list_ptr, int pos_index)
{

    int count = 0;
    llist_elem_t *tem_elem, *L, *R;
    data_t *ret_elem;
    tem_elem = list_ptr->ll_front;
    
    if(list_ptr->ll_entry_count == 0){
        return NULL;
    }
    for(count = 0; count < pos_index; count++){                   // find the position of removed element
            tem_elem = tem_elem->ll_next;
        }
        
    if(tem_elem == NULL){                                         // the the element in this position is NULL, return NULL and not executed remove
        return NULL;
    }
    if(list_ptr->ll_front == tem_elem){                           // if the tem_elem is the first element
        if(list_ptr->ll_entry_count == 1){                          // in this case if there are only one element
            ret_elem = tem_elem->data_ptr;
            list_ptr->ll_front = NULL;
            list_ptr->ll_back = NULL;
            free(tem_elem);
        }
        else{                                                       // in this case there are at least two elements
            ret_elem = tem_elem->data_ptr;
            list_ptr->ll_front = tem_elem->ll_next;
            tem_elem->ll_next->ll_prev = NULL;
            free(tem_elem);
        }
    }
    else if(list_ptr->ll_back == tem_elem){                      // if the tem_elem is the last element
        ret_elem = tem_elem->data_ptr;
        list_ptr->ll_back = tem_elem->ll_prev;
        tem_elem->ll_prev->ll_next = NULL;
        free(tem_elem);
    }
    else{                                                       // normal case , the elements are in L -> tem_elem -> R sequence
        ret_elem = tem_elem->data_ptr;
        L = tem_elem->ll_prev;                                         
        R = tem_elem->ll_next;
        L->ll_next = R;
        R->ll_prev = L;
        free(tem_elem);
    }
    tem_elem = NULL;
    list_ptr->ll_entry_count--;
    /* fix the return value */
    return ret_elem;
}

/* Obtains the length of the specified list, that is, the number of elements
 * that the list contains.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * Returns an integer equal to the number of elements stored in the list.  An
 * empty list has a size of zero.
 */
int llist_entries(llist_t *list_ptr)
{
    assert(list_ptr != NULL);
    assert(list_ptr->ll_entry_count >= 0);
    return list_ptr->ll_entry_count;
}

/* Sort the list.
  Four sort methods and two sort orders available
    *   1.  Insertion sort 
    *   2.  Recursive Selection Sort 
    *   3.  Iterative Selection Sort 
    *   4.  Merge Sort   
    */
 void llist_sort(llist_t *list_ptr, int sort_type, 
      int (*fcomp)(const data_t *, const data_t *))
{
    list_ptr->compare_fun = fcomp;
    if(sort_type == 1){
      int count = llist_entries(list_ptr);
      data_t *new_elem;
      llist_t *new_list_ptr = (llist_t*)calloc(1, sizeof(llist_t));
      new_list_ptr->compare_fun = fcomp;
      new_list_ptr->ll_sorted_state = LLIST_SORTED;
      while(list_ptr->ll_front != NULL){
        new_elem = llist_remove(list_ptr, 0);
        llist_insert_sorted(new_list_ptr, new_elem);
      }
      list_ptr->ll_entry_count = count;
      list_ptr->ll_front = new_list_ptr->ll_front;
      list_ptr->ll_back = new_list_ptr->ll_back;
      free(new_list_ptr);
      new_list_ptr = NULL;
    } else if(sort_type == 2){
      llist_select_sort(list_ptr, list_ptr->ll_front, list_ptr->ll_back);
    } else if(sort_type == 3){
      llist_itera_sort(list_ptr, list_ptr->ll_front, list_ptr->ll_back);
    } else if(sort_type == 4){
      list_ptr = llist_merge_sort(list_ptr);
    } else {
      printf("invalid sort type");
    }
    list_ptr->ll_sorted_state = LLIST_SORTED;
    llist_debug_validate(list_ptr);
}

/* Those function are sort support functions
 */
llist_elem_t *llist_findmax(llist_t *list_ptr, llist_elem_t *m, llist_elem_t *n)
{
  llist_elem_t *i = m;
  llist_elem_t *j = m;
  data_t *prev_data = NULL;
  data_t *next_data = NULL;

  do{
    i = i->ll_next;
    prev_data = i->data_ptr;
    next_data = j->data_ptr;
    if((list_ptr->compare_fun(prev_data, next_data) == 1)){
      j = i;
    }
  } while(i != n);
  return j;
}

void llist_select_sort(llist_t *list_ptr, llist_elem_t *m, llist_elem_t *n)
{
  llist_elem_t *Max_elem;
  data_t *tem_data;
  
  if((llist_entries(list_ptr) > 1) && (m->ll_next != NULL) ){
    Max_elem = llist_findmax(list_ptr, m, n);  //Let Max_elem be the index of the largest in the queue from m to n

    tem_data = m->data_ptr;                                  //Exchange m <--> Max_elem 
    m->data_ptr = Max_elem->data_ptr;
    Max_elem->data_ptr = tem_data;

    llist_select_sort(list_ptr, m->ll_next, n);  //SelectionSort the queue from m->ll_next to n
  }
}        

void llist_itera_sort(llist_t *list_ptr, llist_elem_t *m, llist_elem_t *n)
{
  llist_elem_t *Max_elem;
  data_t *tem_data;
  llist_elem_t *i;
  data_t *comp1_data = m->data_ptr;
  data_t *comp2_data = n->data_ptr;
  while((llist_entries(list_ptr) > 1) && (m->ll_next != NULL)){
    i = m;
    Max_elem = m;

    do{
      i = i->ll_next;
      comp1_data = i->data_ptr;
      comp2_data = Max_elem->data_ptr;
      if((list_ptr->compare_fun(comp1_data,comp2_data) == 1)){
        Max_elem = i;
      }
    } while(i != n);

    tem_data = m->data_ptr;                                  //Exchange m <--> Max_elem 
    m->data_ptr = Max_elem->data_ptr;
    Max_elem->data_ptr = tem_data;

    m = m->ll_next;
  }
} 

llist_t *llist_merge_sort(llist_t *list_ptr)
{
  int count = llist_entries(list_ptr);
  if(count > 1){ 
    llist_t *L, *R;                              // if there are only one element, just return
    L = (llist_t*)calloc(1, sizeof(llist_t));
    R = (llist_t*)calloc(1, sizeof(llist_t));
   
    llist_merge_select(list_ptr, L, R);

    llist_merge_sort(L);                      //sort L and R
    llist_merge_sort(R);
    list_ptr = Merge(list_ptr, L, R);         // merge

    free(L);
    free(R);
    return list_ptr;
  } else{
    return list_ptr;
  }
}

void llist_merge_select(llist_t *list_ptr, llist_t *L, llist_t *R)
{
   int count = llist_entries(list_ptr);
    int i = 0;
    int bp = 0; 
    llist_elem_t *bp_ptr;                         // break point element (rover)
    bp_ptr = list_ptr->ll_front;                  // break point  (rover point)
    L->compare_fun = list_ptr->compare_fun;
    R->compare_fun = list_ptr->compare_fun;
    if(count%2 == 1){                            //If the count is odd
      L->ll_entry_count = count/2 + 1;
      R->ll_entry_count = count/2;
      bp = count/2 + 1;  
    }
    else if(count%2 == 0){                       // else the count is even
     L->ll_entry_count = count/2;
     R->ll_entry_count = count/2;
     bp = count/2;
    }

    L->ll_front = list_ptr->ll_front;
    list_ptr->ll_front = NULL;             //set L and R
    R->ll_back = list_ptr->ll_back;
    list_ptr->ll_back = NULL;
    for(i=0; i < bp; i++){
      bp_ptr = bp_ptr->ll_next;
    }
    L->ll_back = bp_ptr->ll_prev;
    R->ll_front = bp_ptr;
    L->ll_back->ll_next = NULL;
    R->ll_front->ll_prev = NULL;
}

llist_t *Merge(llist_t* list_ptr, llist_t *L, llist_t *R)
{
  list_ptr->ll_entry_count = 0;

  while((L->ll_front != NULL) && (R->ll_front != NULL)){                // if both L and R has one more elments, continue loop
    if(list_ptr->compare_fun(L->ll_front->data_ptr, R->ll_front->data_ptr) == 1){   // If L is the larger one
      llist_merge_insert(list_ptr, L->ll_front);
      L->ll_front = L->ll_front->ll_next;
    }
    else {                                                  // if R is the larger
      llist_merge_insert(list_ptr, R->ll_front);
      R->ll_front = R->ll_front->ll_next;
    }
  }
  if(L->ll_front != NULL){                           // if R is empty, continue loop until L is empty
    while(L->ll_front != NULL){
      llist_merge_insert(list_ptr, L->ll_front);
      L->ll_front = L->ll_front->ll_next;
    }
  }
  else{
     while(R->ll_front != NULL){                   // if L is empty, continue loop until R is empty
      llist_merge_insert(list_ptr, R->ll_front);
      R->ll_front = R->ll_front->ll_next;
     }
  }
  list_ptr->ll_back->ll_next = NULL;               // set the list and return
  return list_ptr;
}

void llist_merge_insert(llist_t *list_ptr, llist_elem_t *tem_elem)
{
  if(list_ptr->ll_entry_count == 0){               // if tem_elem is the first element
    list_ptr->ll_front = tem_elem;
    list_ptr->ll_back = tem_elem;
    list_ptr->ll_entry_count++;
  }
  else{
    list_ptr->ll_back->ll_next = tem_elem;        // if not, place the tem_elem to the end of list
    tem_elem->ll_prev = list_ptr->ll_back;
    list_ptr->ll_back = tem_elem;
    list_ptr->ll_entry_count++;
  }
}


/* This function verifies that the pointers for the two-way linked list are
 * valid, and that the list size matches the number of items in the list.
 *
 * If the linked list is sorted it also checks that the elements in the list
 * appear in the proper order.
 *
 * The function produces no output if the two-way linked list is correct.  It
 * causes the program to terminate and print a line beginning with "Assertion
 * failed:" if an error is detected.
 *
 * The checks are not exhaustive, so an error may still exist in the
 * list even if these checks pass.
 */
void llist_debug_validate(llist_t *L)
{
    llist_elem_t *N;
    int count = 0;
    assert(L != NULL);
    if (L->ll_front == NULL)
        assert(L->ll_back == NULL && L->ll_entry_count == 0);
    if (L->ll_back == NULL)
        assert(L->ll_front == NULL && L->ll_entry_count == 0);
    if (L->ll_entry_count == 0)
        assert(L->ll_front == NULL && L->ll_back == NULL);
    assert(L->ll_sorted_state == LLIST_SORTED || L->ll_sorted_state == LLIST_UNSORTED);

    if (L->ll_entry_count == 1) {
        assert(L->ll_front == L->ll_back && L->ll_front != NULL);
        assert(L->ll_front->ll_next == NULL && L->ll_front->ll_prev == NULL);
        assert(L->ll_front->data_ptr != NULL);
    }
    if (L->ll_front == L->ll_back && L->ll_front != NULL)
        assert(L->ll_entry_count == 1);
    if (L->ll_entry_count > 1) {
        assert(L->ll_front != L->ll_back && L->ll_front != NULL && L->ll_back != NULL);
        N = L->ll_front;
        assert(N->ll_prev == NULL);
        while (N != NULL) {
            assert(N->data_ptr != NULL);
            if (N->ll_next != NULL) assert(N->ll_next->ll_prev == N);
            else assert(N == L->ll_back);
            count++;
            N = N->ll_next;
        }
        assert(count == L->ll_entry_count);
    }
    if (L->ll_sorted_state == LLIST_SORTED && L->ll_front != NULL) {
        N = L->ll_front;
        while (N->ll_next != NULL) {
            assert(L->compare_fun(N->data_ptr, N->ll_next->data_ptr) != -1);
            N = N->ll_next;
        }
    }
}

/* commands for vim. ts: tabstop, sts: softtabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
