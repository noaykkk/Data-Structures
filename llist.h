/* llist.h 
 *
 * Public functions for two-way linked list
 *
 */

#include "datatypes.h"   // defines data_t 

#define LLPOSITION_FRONT -987654
#define LLPOSITION_BACK  -234567

typedef struct llist_element_tag {
    // private members for llist.c only
    data_t *data_ptr;
    struct llist_element_tag *ll_prev;
    struct llist_element_tag *ll_next;
} llist_elem_t;

typedef struct llist_header_tag {
    // private members for llist.c only
    llist_elem_t *ll_front;
    llist_elem_t *ll_back;
    int ll_entry_count;
    int ll_sorted_state;
    // Private method for llist.c only
    int (*compare_fun)(const data_t *, const data_t *);
} llist_t;

/* prototype definitions for functions in llist.c */
data_t *  llist_access(llist_t *list_ptr, int pos_index);
llist_t * llist_construct(int (*fcomp)(const data_t *, const data_t *));
void      llist_destruct(llist_t *list_ptr);
data_t *  llist_elem_find(llist_t *list_ptr, data_t *elem_ptr, int *pos_index,
        int (*fcomp)(const data_t *, const data_t *));
int       llist_entries(llist_t *list_ptr);
void      llist_insert(llist_t *list_ptr, data_t *elem_ptr, int pos_index);
void      llist_insert_sorted(llist_t *list_ptr, data_t *elem_ptr);
data_t *  llist_remove(llist_t *list_ptr, int pos_index);
void      llist_sort(llist_t *list_ptr, int sort_type,
        int (*fcomp)(const data_t *, const data_t *));

/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */