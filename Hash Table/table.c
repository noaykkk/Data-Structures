#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#include "table.h"

/* prototypes for functions in this file only */
unsigned int hash(hashkey_t key);
unsigned int second_hash(hashkey_t key);

/* Dynamically alloct a new table. The size and the probe of the table would be determined by input value table_size and probe_type.
 * The table would not be changed by overflow table entries number.
*/
table_t *table_construct(int table_size, int probe_type){
  // Allocate the table
  table_t *T = (table_t *)malloc(sizeof(table_t));
  // Initialize the table value
  T->table_size_M = table_size;
  T->type_of_probing = probe_type;
  T->num_keys = 0;
  T->num_probes_for_most_recent_call = 0;

  // Determine the probe type and initialize the table
  int i = 0;
  if(probe_type == LINEAR || probe_type == DOUBLE){
    T->oa = (table_entry_t *)malloc(table_size * sizeof(table_entry_t));
    for(i = 0; i < table_size; i++){
      T->oa[i].key = NULL;
      T->oa[i].data_ptr = NULL;
      T->oa[i].deleted = 0;
    }
  }else if(probe_type == CHAIN){
    T->sc = (sep_chain_t **)malloc(table_size * sizeof(sep_chain_t *));
    for(i = 0; i < table_size; i++){
      T->sc[i] = NULL;
    }
   } 

  return T;
}

/* This function would dynamically alloct a new table and move the old table entries data to the new table.
 * The old function would be free after end moving.
 */
table_t *table_rehash(table_t * T, int new_table_size){
  // Allocate the table
  table_t *new_table = table_construct(new_table_size, T->type_of_probing);

  // Allocate the table entries
  int i = 0;
    // For the probe type of table is LINEAR and DOUBLE
  if(T->type_of_probing == LINEAR || T->type_of_probing == DOUBLE){
    for(i = 0; i < T->table_size_M; i++){
      if(T->oa[i].key != NULL){
        table_insert(new_table, T->oa[i].key, T->oa[i].data_ptr);
        T->oa[i].key = NULL;
        T->oa[i].data_ptr = NULL;
      }
    }
  }
    // For the probe type of table is CHAIN
  else if(T->type_of_probing == CHAIN){
    sep_chain_t *temp, *succ;
    for(i = 0; i < T->table_size_M; i++){
      if(T->sc[i] != NULL){
        table_insert(new_table, T->sc[i]->key, T->sc[i]->data_ptr);
        T->sc[i]->key = NULL;
        T->sc[i]->data_ptr = NULL;
        // If the table entry has next entry, continue table_rehash
        if(T->sc[i]->next != NULL){
          temp = T->sc[i]->next;
          T->sc[i]->next = NULL;
          while(temp != NULL){
            table_insert(new_table, temp->key, temp->data_ptr);
            temp->key = NULL;
            temp->data_ptr = NULL;
            succ = temp;
            temp = temp->next;
            free(succ);
          }
        }
      }
    }
  }

  // Free the old table and return new table
  table_destruct(T);
  return new_table;
}

/* Function to return how many table entries storage in the table
 */
int table_entries(table_t *T){
  return T->num_keys;
}

/* Function to check the table is full or not.
 * If full, return 1; otherwise return 0.
 */
int table_full(table_t *T){
  
  // If only 1 entry is empty, the table is full, return 1
  if(T->table_size_M - T->num_keys == 1){
    return 1;
  }else{
    return 0;
  }
}

/* Function to see how many table entries are marked as deleted.
 * Return the count number.
 */
int table_deletekeys(table_t *T){
  if(T->type_of_probing == LINEAR || T->type_of_probing == DOUBLE){
  int count = 0, i = 0;

  for (i = 0; i < T->table_size_M; i++){
    if(T->oa[i].deleted == 1){
      count++;
    }
  }
  return count;
  }
  return 0;
}

/* Function to insert a new table entry(K,I) into the table
 * Return 0 if insertion successful
 * Return 1 if the (K,I) is already in the table, in this case old I would be replaced with new I
 * Return -1 if (K,I) cannot be inserted
 */
int table_insert(table_t *T, hashkey_t K, data_t I){
  T->num_probes_for_most_recent_call = 0;
  // Calculate the hash value and reminder
  bool D = false; // To determine the K is already in the table or not
  bool R = false; // To determine the return value if K is already in the table
  int dummy = -1;
  unsigned int hash_value = hash(K);
  int remainder = hash_value % T->table_size_M;
  if(remainder < 0){
      remainder = remainder + T->table_size_M;
  }

  // If the table is open address
  if(T->type_of_probing == LINEAR || T->type_of_probing == DOUBLE){
    // If the table probe is LINEAR
    if(T->type_of_probing == LINEAR){
      // If it is not empty, it is a collision. Linear find the table entry and count the num_probes_for_most_recent_call
        // Search for the position   
        while(T->oa[remainder].deleted == 1 || (T->oa[remainder].key != NULL && strcmp(K, T->oa[remainder].key) != 0)){
          if(T->oa[remainder].deleted == 1 && D == false){
            D = true;
            dummy = remainder;
          }
          remainder--;
          T->num_probes_for_most_recent_call++;
          if(remainder < 0){
            remainder = T->table_size_M + remainder;
          }
          if(T->num_probes_for_most_recent_call >= T->table_size_M){
            break;
          }
        }
    }

    // If the table is DOUBLE
    else if(T->type_of_probing == DOUBLE){
      unsigned int second_hash_value = second_hash(K);
      int second_probe = second_hash_value % T->table_size_M;
      if(second_probe == 0){
        second_probe = 1;
      } 
       // If it is not empty, it is a collision. DOUBLE hash find the table entry and count the num_probes_for_most_recent_call
         // Search for the position   
         while(T->oa[remainder].deleted == 1 || (T->oa[remainder].key != NULL && strcmp(K, T->oa[remainder].key) != 0)){
           if(T->oa[remainder].deleted == 1 && D == false){
            D = true;
            dummy = remainder;
          }
          remainder -= second_probe;
          T->num_probes_for_most_recent_call++;
           if(remainder < 0){
             remainder = T->table_size_M + remainder;
           }
           if(T->num_probes_for_most_recent_call >= T->table_size_M){
             break;
           }
        }
    }
  
    // Fill the table entry
    T->num_probes_for_most_recent_call++;
    if(table_full(T) == 1 && dummy == -1 && T->oa[remainder].key == NULL){
      return -1;
    }
    if(T->oa[remainder].key != NULL && strcmp(K, T->oa[remainder].key) == 0){
      free(T->oa[remainder].key);
      free(T->oa[remainder].data_ptr);
      T->oa[remainder].key = K;
      T->oa[remainder].data_ptr = I;
      T->oa[remainder].deleted = 0;
      R = true;
    } else if(D == true){
      T->oa[dummy].key = K;
      T->oa[dummy].data_ptr = I;
      T->oa[dummy].deleted = 0;
    } else{
      T->oa[remainder].key = K;
      T->oa[remainder].data_ptr = I;
      T->oa[remainder].deleted = 0;
    }   
  }



  // If the table is separate chaining
  else if(T->type_of_probing == CHAIN){
    T->num_probes_for_most_recent_call = 1;
    if(T->sc[remainder] == NULL && table_full(T) == 1){
      return -1;
    }
    // If the table entry is empty
    else if(T->sc[remainder] == NULL && table_full(T) != 1){
      T->sc[remainder] = (sep_chain_t*)malloc(sizeof(sep_chain_t));
      T->sc[remainder]->key = K;
      T->sc[remainder]->data_ptr = I;
      T->sc[remainder]->next = NULL;
    }else{
      // Find the last postion the the table
      T->num_probes_for_most_recent_call++;
      sep_chain_t *Rover = T->sc[remainder];
      while(Rover->next != NULL && strcmp(K, Rover->key) != 0){
        Rover = Rover->next;
        T->num_probes_for_most_recent_call++;
      }
      // If K has already in the table, update the data
      if(Rover->key != NULL && strcmp(K, Rover->key) == 0){
        free(Rover->key);
        free(Rover->data_ptr);
        Rover->key = K;
        Rover->data_ptr = I;
        R = true;
      }else{
        // Allocate a new table entry, fill it and link it in table 
        sep_chain_t *new_entry = (sep_chain_t*)malloc(sizeof(sep_chain_t));
        new_entry->key = K;
        new_entry->data_ptr = I;
        new_entry->next = NULL;
        Rover->next = new_entry;
      } 
    }    
  }

  // If the K is already in the table, return 1
  if(R == true){
    return 1;
  }else{
    T->num_keys++;
    return 0;
  }
}

//
data_t table_delete(table_t *T, hashkey_t K){
  T->num_probes_for_most_recent_call = 0;
  unsigned int hash_value = hash(K);
  int remainder = hash_value % T->table_size_M;
  if(remainder < 0){
      remainder = remainder + T->table_size_M;
  }

  // If the table is open address
  if(T->type_of_probing == LINEAR || T->type_of_probing == DOUBLE){
    // For the probe type of LINEAR
    if(T->type_of_probing == LINEAR){
      // Search for the position
      while(T->oa[remainder].deleted == 1 || (T->oa[remainder].key != NULL && strcmp(K, T->oa[remainder].key) != 0)){
        remainder --;
        T->num_probes_for_most_recent_call++;
        if(remainder < 0){
          remainder = T->table_size_M + remainder;
        }
        if(T->num_probes_for_most_recent_call >= T->table_size_M){
          break;
        }
      }
    }

    // For the probe type of DOUBLE
    if(T->type_of_probing == DOUBLE){
      unsigned int second_hash_value = second_hash(K);
      int second_probe = second_hash_value % T->table_size_M;
      if(second_probe == 0){
        second_probe = 1;
      }
      // Search for the position
      while(T->oa[remainder].deleted == 1 || (T->oa[remainder].key != NULL && strcmp(K, T->oa[remainder].key) != 0)){
        remainder -= second_probe;
        T->num_probes_for_most_recent_call++;
        if(remainder < 0){
          remainder = T->table_size_M + remainder;
        }
        if(T->num_probes_for_most_recent_call >= T->table_size_M){
          break;
        }
      }
    }
    T->num_probes_for_most_recent_call++;

    // If travel to the empty table entry but still not find key, return NULL
    if(T->oa[remainder].key == NULL || strcmp(K, T->oa[remainder].key) != 0){
      return NULL;
    }
    // The deleted table entry is found, free the key and mark it as deleted
    data_t *ret_data = T->oa[remainder].data_ptr;
    free(T->oa[remainder].key);
    T->oa[remainder].key = NULL;
    T->oa[remainder].data_ptr = NULL;
    T->oa[remainder].deleted = 1;
    T->num_keys--;
    return ret_data;
  
  }

  // For the probe type of CHAIN
  else if(T->type_of_probing == CHAIN){
    // Find the position of table entry
    sep_chain_t *successor = T->sc[remainder];
    sep_chain_t *Rover = T->sc[remainder];
    if(Rover == NULL){
      return NULL;
      }
    while(Rover != NULL && strcmp(K, Rover->key) != 0){
      successor = Rover;
      Rover = Rover->next;
      T->num_probes_for_most_recent_call++;
    }

    // If find the table entry, free the entry and return I
    if(Rover != NULL && strcmp(K, Rover->key) == 0){
      data_t *temp = Rover->data_ptr;
      free(Rover->key);
      Rover->key = NULL;
      Rover->data_ptr = NULL;
      // If table entries are linked as successor -> Rover -> Rover->next
      // Linked them as successor -> Rover->next whether Rover->next is a table entry or NULL
      if(successor != Rover){
        successor->next = Rover->next;
        free(Rover);
      }else if(Rover->next != NULL){
        // If successor is the same as Rover, otherwise T->sc[remainder] = Rover
        // whether Rover->next is a table entry or NULL, use it replace Rover
        T->sc[remainder] = Rover->next;
        free(Rover);
      } else{
        free(Rover);
        T->sc[remainder] = NULL;
      }
      T->num_keys--;
      return temp;
    }
  }
  return NULL;
}

/* Function to retrieve the pointer to the information by given key.
 * Return NULL when key is not found
 */
data_t table_retrieve(table_t *T, hashkey_t K){
  T->num_probes_for_most_recent_call = 0;
  unsigned int hash_value = hash(K);
  int remainder = hash_value % T->table_size_M;
  if(remainder < 0){
      remainder = remainder + T->table_size_M;
  }

  // If the table is open address
  if(T->type_of_probing == LINEAR || T->type_of_probing == DOUBLE){
    // For the probe type of LINEAR
    if(T->type_of_probing == LINEAR){
      // Search for the position
      while(T->oa[remainder].deleted == 1 || (T->oa[remainder].key != NULL && strcmp(K, T->oa[remainder].key) != 0)){
        remainder --;
        T->num_probes_for_most_recent_call++;
        if(remainder < 0){
          remainder = T->table_size_M + remainder;
        }
        if(T->num_probes_for_most_recent_call >= T->table_size_M){
          break;
        }
      }
    }

    // For the probe type of DOUBLE
    if(T->type_of_probing == DOUBLE){
      unsigned int second_hash_value = second_hash(K);
      int second_probe = second_hash_value % T->table_size_M;
      if(second_probe == 0){
        second_probe = 1;
      }
      // Search for the position
      while(T->oa[remainder].deleted == 1 || (T->oa[remainder].key != NULL && strcmp(K, T->oa[remainder].key) != 0)){
        remainder -= second_probe;
        T->num_probes_for_most_recent_call++;
        if(remainder < 0){
          remainder = T->table_size_M + remainder;
        }
        if(T->num_probes_for_most_recent_call >= T->table_size_M){
          break;
        }
      }
    }
    T->num_probes_for_most_recent_call++;

    // If travel to the empty table entry but still not find key, return NULL
    if(T->oa[remainder].key == NULL || strcmp(K, T->oa[remainder].key) != 0){
      return NULL;
    }
    // The table entry is found
    return T->oa[remainder].data_ptr; 
  }

  // For the probe type of CHAIN
  else if(T->type_of_probing == CHAIN){
    // Find the position of table entry
    sep_chain_t *Rover = T->sc[remainder];
    while(Rover != NULL && strcmp(K, Rover->key) != 0){
      Rover = Rover->next;
      T->num_probes_for_most_recent_call++;
    }

    // If find the table entry, free the entry and return I
    if(Rover != NULL && Rover->key != NULL && strcmp(K, Rover->key) == 0){
      T->num_probes_for_most_recent_call++;
      return Rover->data_ptr;
    }else{
      // If the table entry is not in the table, return NULL
      return NULL;
    }
  }
  return NULL;
}

/* Function to free the table
 */
void table_destruct(table_t *T){
  int i = 0;

  // For probe_type is LINEAR and DOUBLE table
  if(T->type_of_probing == LINEAR || T->type_of_probing == DOUBLE){
    for(i = 0; i < T->table_size_M; i++){
      if(T->oa[i].key != NULL){
        free(T->oa[i].key);
        free(T->oa[i].data_ptr);
      }
    }
    free(T->oa);
  }
  // For probe_type is CHAIN table
  if(T->type_of_probing == CHAIN){
    sep_chain_t *temp, *succ;
    for(i = 0; i < T->table_size_M; i++){
      // If the table entry is not empty
      if(T->sc[i] != NULL){
        free(T->sc[i]->key);
        free(T->sc[i]->data_ptr);
        // If the table has next entry, free the entry until the last entry
        if(T->sc[i]->next != NULL){
          temp = T->sc[i]->next;
          while(temp != NULL){
            free(temp->key);
            free(temp->data_ptr);
            succ = temp;
            temp = temp->next;
            free(succ);
          }
        }
      }
      free(T->sc[i]);
    }
    free(T->sc);
  }

  free(T);
}

//
int table_stats(table_t *T){
  return T->num_probes_for_most_recent_call;
}

//
hashkey_t table_peek(table_t *T, int index, int list_position){
  assert(0 <= index && index < T->table_size_M);
  assert(0 <= list_position);
  // For open address table
  if(T->type_of_probing == LINEAR || T->type_of_probing == DOUBLE){
    if(T->oa[index].data_ptr == NULL){
      return 0;
    }else{
      return T->oa[index].key;
    }
  }
  // For separate chaining table
  if(T->type_of_probing == CHAIN){
    if(T->sc[index] == NULL){
      return 0;
    }else{
      int i = 0;
      sep_chain_t *temp = T->sc[index];
      for(i = 0; i < list_position; i++){
        if(temp == NULL){
          return 0;
        }
        temp = temp->next;
      }
      if(temp == NULL){
        return NULL;
        }  
      return temp->key;
    }
  }
  return 0;
}

//
void table_debug_print(table_t *T){
  int i = 0;
  // Part for print open address table
  if(T->type_of_probing == LINEAR || T->type_of_probing == DOUBLE){
    for(i = 0; i < T->table_size_M; i++){
      if(T->oa[i].key == NULL){
        printf("Table entry [%d] is empty. \n", i);
      }else{
        printf("Table entry [%d] has key : %s.", i, T->oa[i].key);
        if(T->oa[i].deleted == 1){
          printf(" This index is deleted");
        }
        printf("\n");
      }
    }
  }
  // Part for print separate chaining table
  if(T->type_of_probing == CHAIN){
    sep_chain_t *temp = NULL;
    for(i = 0; i < T->table_size_M; i++){
      if(T->sc[i] == NULL){
        printf("Table entry [%d] is empty. \n", i);
      }else{
        printf("Table entry [%d] has key :", i);
        temp = T->sc[i];
        while(temp != NULL){
          printf(" %s", temp->key);
          temp = temp->next;
        }
        printf(".\n");
      }
    }
  }
}

/* Function to calculate the hash value
 * The hash fucntion references the Bernstein hash which created by Dan Bernstein
 */
unsigned int hash(hashkey_t key){
  int len = strlen(key);
  unsigned char *p = (unsigned char *)key;
  unsigned int h = 0;
  int i ;

  for(i = 0; i < len; i++){
    h = 33 *h + p[i];
  }

  return h;
}
/* Function to calculate the second hash value which used in double type tabble
 * The hash function also references the Bernstein hash which created by Dan Bernstein
 */
unsigned int second_hash(hashkey_t key){
  int len = strlen(key);
  unsigned char *p = (unsigned char *)key;
  unsigned h = 0;
  int i;

  for (i = 0; i < len; i++)
  {
      h = 33 * h ^ p[i];
  }

  return h;
}
