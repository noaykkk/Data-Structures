/* Assumptions: suggested structure definitions and public functions 
 *              as defined for assignmnet.  
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "llist.h"
#include "ids_support.h"

#define MAXLINE 256

// private functions for use in ids_support.c only
void ids_record_fill(alert_t *rec);   // collect input from user
void ids_print_alert_rec(alert_t *rec);  // print one record

/* ids_compare_genid is required by the list ADT for sorted lists. 
 *
 * This function returns 
 *     1 if rec_a should be closer to the front than rec_b,
 *    -1 if rec_b is to be considered closer to the front, and 
 *     0 if the records are equal for sorting.
 *
 * For the alert data we want to sort from lowest ID up, so
 * closer to the front means a smaller ID.
 *
 * The function expects pointers to two record structures, and it is an error
 * if either is NULL
 *
 */
int ids_compare_genid(const alert_t *rec_a, const alert_t *rec_b)
{
    assert(rec_a != NULL && rec_b != NULL);
    if (rec_a->generator_id < rec_b->generator_id)
	return 1;
    else if (rec_a->generator_id > rec_b->generator_id)
	return -1;
    else
	return 0;
}

int ids_compare_destip(const alert_t *rec_a, const alert_t *rec_b)
{
    assert(rec_a != NULL && rec_b != NULL);
    if (rec_a->dest_ip_addr < rec_b->dest_ip_addr)
	return -1;
    else if (rec_a->dest_ip_addr > rec_b->dest_ip_addr)
	return 1;
    else
	return 0;
}

/* ids_match_destip is used with llist_elem_find. 
 *
 * This function returns 
 *     0 if the records have the same destination IP address.
 *     1 if the destination IP addresses are not equal
 *
 * The function expects pointers to two record structures, and it is an error
 * if either is NULL
 */
int ids_match_destip(const alert_t *rec_a, const alert_t *rec_b)
{
    assert(rec_a != NULL && rec_b != NULL);
    if (rec_a->dest_ip_addr == rec_b->dest_ip_addr)
	return 0;
    else
	return 1;
}

/* print one of the alert record lists
 *
 * inputs: 
 *    list_ptr: a pointer to either sorted or unsorted list
 *
 *    type_of_list: a charter string that must be either "List" or "Queue"
 *
 * output: prints the list in the format required for grading.
 */
void ids_print(llist_t *list_ptr, const char *list_type)   
{
    assert(strcmp(list_type, "List")==0 || strcmp(list_type, "Queue")==0);
    int num_in_list = list_ptr->ll_entry_count;     
    int index;
    if (num_in_list == 0) {                                    // check the list is empty or not
        printf("%s is empty\n", list_type);
    } else {
        printf("%s contains %d record%s\n", list_type, num_in_list,      //if not, printf
                num_in_list==1 ? "." : "s.");
    }
    for (index = 0; index < num_in_list; index++) {
        printf("%d: ", index+1);
        // you must use the ids_print_rec function to format output 
        alert_t *rec_ptr = llist_access(list_ptr, index);   
        ids_print_alert_rec(rec_ptr);
    }
    printf("\n");
}

/* This functions adds an alert record to the rear of a list.  The list is
 * unsorted, and it does not have any limits on the maximum size of the list.
 *
 * If the new alert has the same destination IP address as an alert that is
 * already in the list, the old matching alert is removed from the list and 
 * recycled.  The new alert is appended to the end of the list.
 *
 */
void ids_add_rear(llist_t *list_ptr)               
{
    alert_t *new_ptr = (alert_t *) calloc(1, sizeof(alert_t));
    alert_t *ret_ptr = NULL;
    ids_record_fill(new_ptr);
    // print one of the two following lines
    // if did not find a match in the list
    int position;
    alert_t *old_ptr = NULL;
    old_ptr = llist_elem_find(list_ptr, new_ptr, &position, ids_match_destip);
    if(old_ptr == NULL){
        llist_insert(list_ptr, new_ptr, llist_entries(list_ptr));
        printf("Appended %d onto queue\n", new_ptr->dest_ip_addr);
    }
    
    // else if found and removed a match in the list
    else{
        ret_ptr = llist_remove(list_ptr, position);
        llist_insert(list_ptr, new_ptr, llist_entries(list_ptr));
        printf("Appended %d onto queue and removed old copy\n", new_ptr->dest_ip_addr);
        free(ret_ptr);
        ret_ptr = NULL;
    }
}

/* This function removes the alert record at the front of the queue.  The
 * queue is unsorted, and there is no limit on the maximum size of the list.
 *
 * The alert memory block should be recycled so there are no memory leaks.
 */
void ids_remove_front(llist_t *list_ptr)         
{
    alert_t *rec_ptr = NULL;
    rec_ptr = llist_remove(list_ptr, 0);

    if (rec_ptr != NULL)
        printf("Deleted front with IP addr: %d\n", rec_ptr->dest_ip_addr);
    else
        printf("Queue empty, did not remove\n");
    free(rec_ptr);
    rec_ptr = NULL;
}

/* This creates a list and it can be either a sorted or unsorted list.
 * If the list is unsorted, a comparison function is not needed.  
 *
 * inputs: 
 *    list_type: a character string that must be either "List" or "Queue"
 */
llist_t *ids_create(const char *list_type)
{
    assert(strcmp(list_type, "List")==0 || strcmp(list_type, "Queue")==0);
    if (strcmp(list_type, "List") == 0) {
        return llist_construct(ids_compare_genid);
    } else if (strcmp(list_type, "Queue") == 0) {
        return llist_construct(NULL);
    } else {
        printf("ERROR, invalid list type %s\n", list_type);
        exit(1);
    }
}

/* This function adds an alert to the sorted list.  
 * If the new alert has the same generator ID as one or
 * more alerts in the list, then the new alert is placed after all the
 * existing alerts with the same address. 
 *
 * There is no return value since the insertion must always be 
 * successful, except in the catastrophic condition that the program
 * has run out of memory.
 *
 */

void ids_add(llist_t *list_ptr)         
{
    alert_t *new_ptr = (alert_t *) calloc(1, sizeof(alert_t));
    ids_record_fill(new_ptr);
    
    llist_insert_sorted(list_ptr, new_ptr);
    // after the alert is added you must print
    printf("Inserted %d into list\n", new_ptr->generator_id);
}

/* This function prints all alerts with the matching generator id in the
 * sorted list.  Print all matching alerts, and after the last print the 
 * number found.
 */
void ids_list_gen(llist_t *list_ptr, int gen_id)       
{
    int count = 0, i = 0;
    alert_t *new_ptr;
    // First, you must print each of the matching alerts 
    for(i=0; i < llist_entries(list_ptr); i++){
        new_ptr = llist_access(list_ptr, i);
        if(new_ptr == NULL){                           // if at the end of list, quit loop
            break;
        }
        else if(new_ptr->generator_id == gen_id){     // if equal, count+1
            ids_print_alert_rec(new_ptr);
            count++;
        }
        else if(new_ptr->generator_id > gen_id){    // if at the end of same gen_id sequence, quit loop
            break;
        }
    }
    
    // after printing each matching record, print one of these summaries
    if (count > 0)
        printf("Found %d alerts matching generator %d\n", count, gen_id);
    else
        printf("Did not find alert: %d\n", gen_id);
}

/* This function prints all alerts with the matching destination ip_address in the
 * sorted list.  Print all matching alerts, and after the last print the 
 * number found.
 */
void ids_list_ip(llist_t *list_ptr, int dest_ip)    
{
    int count = 0, i = 0;
    alert_t *new_ptr;
    // First, you must print each of the matching alerts 
    for(i=0; i < llist_entries(list_ptr); i++){
        new_ptr = llist_access(list_ptr, i);
        if(new_ptr == NULL){                          // if at the end of list, quit loop
            break;
        }
        else if(new_ptr->dest_ip_addr == dest_ip){     // if equal, count+1
            ids_print_alert_rec(new_ptr);
            count++;
        }
        else if(new_ptr->dest_ip_addr > dest_ip){     // if at the end of same dest_ip sequence, quit loop
            break;
        }
    }
    // after printing each matching record, print one of these summaries
    if (count > 0)
        printf("Found %d alerts matching IP %d\n", count, dest_ip);
    else
        printf("Did not find destination IP: %d\n", dest_ip);
}

/* This function removes all alerts from the sorted list with the matching
 * generator id. 
 */
void ids_remove_gen(llist_t *list_ptr, int gen_id)    
{
    int count = 0, i = 0;
    alert_t *new_ptr;
    alert_t *ret_ptr;
    
    for(i=0; i < llist_entries(list_ptr); i++){
        new_ptr = llist_access(list_ptr, i);
        if(new_ptr == NULL){                     // if at the end of list, quit loop
            break;
        }
        else if(new_ptr->generator_id == gen_id){    // if equal, count+1
            ret_ptr = llist_remove(list_ptr, i);
            free(ret_ptr);
            ret_ptr = NULL;
            count++;
            i--;
        }
        else if(new_ptr->generator_id > gen_id){  // if at the end of same gen_id sequence, quit loop
            break;
        }
    }
    if (count > 0)
        printf("Removed %d alerts matching generator %d\n", count, gen_id);
    else
        printf("Did not remove alert with generator: %d\n", gen_id);
}

/* This function removes all alerts from the sorted list with the matching
 * generator id. 
 */
void ids_remove_ip(llist_t *list_ptr, int dest_ip)   
{
    int count = 0, i = 0;
    alert_t *new_ptr;
    alert_t *ret_ptr;
    // First, you must print each of the matching alerts 
    for(i=0; i < llist_entries(list_ptr); i++){
        new_ptr = llist_access(list_ptr, i);
        if(new_ptr == NULL){                        // if at the end of list, quit loop
            break;
        }
        else if(new_ptr->dest_ip_addr == dest_ip){   // if equal, count+1
            ret_ptr = llist_remove(list_ptr, i);
            free(ret_ptr);
            ret_ptr = NULL;
            count++;
            i--;
        }
        else if(new_ptr->dest_ip_addr > dest_ip){    // if at the end of same dest_ip sequence, quit loop
            break;
        }
    }
    if (count > 0)
        printf("Removed %d alerts matching IP %d\n", count, dest_ip);
    else
        printf("Did not remove alert with IP: %d\n", dest_ip);
}

void ids_scan(llist_t *list_ptr, int thresh)        
{
    int count = 0, i;
    int found_addr = -1;
    int sets = 0;
    alert_t *new_ptr;
    // for each set that is found print the following line
    for(i=0; i < llist_entries(list_ptr); i++){                    
                new_ptr = llist_access(list_ptr, i);
                if(new_ptr == NULL){
                    if(count >= thresh){
                      printf("A set with generator %d has %d alerts\n", found_addr, count);
                      sets++;
                      break;
                    }                     // If it is NULL, jump next
                    continue;
                }
                if( found_addr == new_ptr->generator_id){  // If not, compare the id, if they have the same ID nunber, index++ 
                  count++;
                  continue;
                }
                else if(count >= thresh){        // If not, check the match times. If so, print then reset the index and id number
                  printf("A set with generator %d has %d alerts\n", found_addr, count);            // for each group that is found print the number of matches
                  sets++;                  
                }
                count = 1;
                found_addr = new_ptr->generator_id;
            }
 
    // after all sets have been discovered print one of the following
    if (sets > 0) 
        printf("Scan found %d sets\n", sets);
    else
        printf("Scan found no alerts with >= %d matches\n", thresh);
}

/* the function takes a pointer to each list and prints the
 * number of items in each list
 */
void ids_stats(llist_t *sorted, llist_t *unsorted)
{
    // get the number in list and size of the list
    int num_in_sorted_list = llist_entries(sorted);
    int num_in_unsorted_list = llist_entries(unsorted);
    printf("Number records in list: %d, queue size: %d\n", 
            num_in_sorted_list, num_in_unsorted_list);
}

/* this function frees the memory for either a sorted or unsorted list.
 */
void ids_cleanup(llist_t *list_ptr)      
{
    llist_destruct(list_ptr);
}

/* this function sorts the queue in ascending order.
 */
void ids_sortgen(llist_t *list_ptr, int sort_type)
{
  clock_t start, end;
  double elapse_time; /* time in milliseconds */

  int initialcount = llist_entries(list_ptr);
  start = clock();
  llist_sort(list_ptr, sort_type, ids_compare_genid);
  end = clock();
  elapse_time = 1000.0 * ((double) (end - start)) / CLOCKS_PER_SEC;
  assert(llist_entries(list_ptr) == initialcount);
  printf("%d\t%f\t%d\n", initialcount, elapse_time, sort_type);
}

/* this function sorts the queue in decending order.
 */
void ids_sortip(llist_t *list_ptr, int sort_type)
{
  clock_t start, end;
  double elapse_time; /* time in milliseconds */

  int initialcount = llist_entries(list_ptr);
  start = clock();
  llist_sort(list_ptr, sort_type, ids_compare_destip);
  end = clock();
  elapse_time = 1000.0 * ((double) (end - start)) / CLOCKS_PER_SEC;
  assert(llist_entries(list_ptr) == initialcount);
  printf("%d\t%f\t%d\n", initialcount, elapse_time, sort_type);
}

void ids_app_rear(llist_t *list_ptr, int gen_id, int dest_ip)
{
  alert_t *new_ptr = (alert_t *) calloc(1, sizeof(alert_t));
  new_ptr->generator_id = gen_id;
  new_ptr->dest_ip_addr = dest_ip;
  llist_insert(list_ptr, new_ptr, LLPOSITION_BACK);
}
/* Prompts user for alert record input starting with the generator ID.
 * The input is not checked for errors but will default to an acceptable value
 * if the input is incorrect or missing.
 *
 * The input to the function assumes that the structure has already been
 * created.  The contents of the structure are filled in.
 *
 * There is no output.
 *
 * Do not change the sequence of prompts as grading requires this exact
 * format
 */
void ids_record_fill(alert_t *rec)
{
    char line[MAXLINE];
    assert(rec != NULL);

    printf("Generator component:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &rec->generator_id);
    printf("Signature:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &rec->signature_id);
    printf("Revision:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &rec->revision_id);
    printf("Dest IP address:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &rec->dest_ip_addr);
    printf("Source IP address:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &rec->src_ip_addr);
    printf("Destination port number:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &rec->dest_port_num);
    printf("Source port number:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &rec->src_port_num);
    printf("Time:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &rec->timestamp);
    printf("\n");
}

/* print the information for a particular alert record 
 *
 * Input is a pointer to a record, and no entries are changed.
 */
void ids_print_alert_rec(alert_t *rec)
{
    assert(rec != NULL);
    printf("[%d:%d:%d] (gen, sig, rev): ", rec->generator_id, rec->signature_id, 
            rec->revision_id);
    printf("Dest IP: %d, Src: %d, Dest port: %d,", rec->dest_ip_addr, 
            rec->src_ip_addr, rec->dest_port_num);
    printf(" Src: %d, Time: %d\n", rec->src_port_num, rec->timestamp); 
}

/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */

