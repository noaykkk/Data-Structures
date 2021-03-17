/* table.h 
 * Version: 1
 */

/* constants used to indicate type of probing.  */
enum ProbeDec_t {LINEAR, DOUBLE, CHAIN};

typedef void *data_t;   /* pointer to the information, I, to be stored in the table */
typedef char *hashkey_t;   /* the key, K, for the pair (K, I) */

typedef struct sep_chain_tag {
    hashkey_t key;
    data_t data_ptr;
    struct sep_chain_tag *next;
} sep_chain_t;

typedef struct table_etag {
    hashkey_t key;
    data_t data_ptr;
    int deleted;
} table_entry_t;

typedef struct table_tag {
    // you need to fill in details, and you can change the names!
    int table_size_M;
    int type_of_probing_used_for_this_table;
    int num_keys_stored_in_table;
    int num_probes_for_most_recent_call;
    table_entry_t *oa;
    sep_chain_t **sc;
} table_t;

/*  The empty table is created.  The table must be dynamically allocated and
 *  have a total size of table_size.  The maximum number of (K, I) entries
 *  that can be stored in the table is table_size-1.  For open addressing, 
 *  the table is filled with a special empty key distinct from all other 
 *  nonempty keys (e.g., NULL).  
 *
 *  the probe_type must be one of {LINEAR, DOUBLE, CHAIN}
 *
 *  Do not "correct" the table_size or probe decrement if there is a chance
 *  that the combinaion of table size or probe decrement will not cover
 *  all entries in the table.  Instead we will experiment to determine under
 *  what conditions an incorrect choice of table size and probe decrement
 *  results in poor performance.
 */
table_t *table_construct(int table_size, int probe_type);  

/* Sequentially remove each table entry (K, I) and insert into a new
 * empty table with size new_table_size.  Free the memory for the old table
 * and return the pointer to the new table.  The probe type should remain
 * the same.
 *
 * Do not rehash the table during an insert or delete function call.  Instead
 * use drivers to verify under what conditions rehashing is required, and
 * call the rehash function in the driver to show how the performance
 * can be improved.
 */
table_t *table_rehash(table_t * T, int new_table_size);  

/* returns number of entries in the table */
int table_entries(table_t *);

/* returns 1 if table is full and 0 if not full. 
 * For separate chaining the table is never full
 */
int table_full(table_t *);

/* returns the number of table entries marked as deleted */
int table_deletekeys(table_t *);
   
/* Insert a new table entry (K, I) into the table provided the table is not
 * already full.  
 * Return:
 *      0 if (K, I) is inserted, 
 *      1 if an older (K, I) is already in the table (update with the new I), or 
 *     -1 if the (K, I) pair cannot be inserted.
 *
 * Note that both K and I are pointers to memory blocks created by malloc()
 */
int table_insert(table_t *, hashkey_t K, data_t I);

/* Delete the table entry (K, I) from the table.  Free the key in the table.
 * Return:
 *     pointer to I, or
 *     null if (K, I) is not found in the table.  
 *
 * Be sure to free(K)
 *
 * See the note on page 490 in Standish's book about marking table entries for
 * deletions when using open addressing.
 */
data_t table_delete(table_t *, hashkey_t K); 

/* Given a key, K, retrieve the pointer to the information, I, from the table,
 * but do not remove (K, I) from the table.  Return NULL if the key is not
 * found.
 */
data_t table_retrieve(table_t *, hashkey_t K); 

/* Free all information in the table, the table itself, and any additional
 * headers or other supporting data structures.  
 */
void table_destruct(table_t *);

/* The number of probes for the most recent call to table_retrieve,
 * table_insert, or table_delete 
 */
int table_stats(table_t *);  

/* This function is for testing purposes only.  Given an index position into
 * the hash table return the value of the key if data is stored in this 
 * index position.  If the index position does not contain data, then the
 * return value must be zero.  Make the first
 * lines of this function 
 *       assert(0 <= index && index < table_size); 
 *       assert(0 <= list_position); 
 */
hashkey_t table_peek(table_t *T, int index, int list_position); 

/* Print the table position and keys in a easily readable and compact format.
 * Only useful when the table is small.
 */
void table_debug_print(table_t *T);

