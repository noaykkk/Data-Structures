# Sort-two-way-linked-list-in-C
Data structures:
Implement four ways to sort a two-way linked list. (Merge, insert by function, recursive and iterative)
This program is an abstract data type, for its definition can be found in https://en.wikipedia.org/wiki/Abstract_data_type
The datatype is modeled one portion of the honeypot. The Honeypot is one strategy used by computer network administrators to monitor and isolate unauthorized use of information systems. For more details and definitions please move forward to datatypes.h
ids_support.c is the implement of program functions
Only llist.c has permission to make changes to the linked list

Extended function for program:

For sorted list only:
ALERT: create a dynamic memory block then add it in list after filled
LISTGEN gen_id: print the information for block which generator_id matches the input gen_ip
LISTIP ip-address: operates the same way except looks for records for which the input value ip-address matches the dest_ip_addr in the alert records
DELGEN gen-id: removes all the corresponding records for which the generator_id matches the input value gen-id from the list, frees the memory, and uses the supplied print to                    report the number of alerts removed. If the item was not found, in which case use the print the reports no matches were found
DELIP ip-address: operates the same way except looks for records for which the input value ip-address matches the dest_ip_addr in the alert records
SCANGEN threshold: searches the list for all sets of alerts for which there are threshold ormore alert records with the same generator_id
PRINT: outputs the count of alerts in the list and the current size of the database followed by the contents of each alert

For unsorted list (queue) only:
ADDREAR: first checks if a packet with the same destination ip_address is already in the list. If so, the packet is removed and freed. In any case, the new packet is appended to            the end of the list
RMFRONT: simply removes the record at the front of the list
STATS: prints the size of both the sorted and unsorted lists
SORTGEN x: sorts the queue in ascending order based on generator_id 
           ( x is one of the four sorting algorithms )
SORTIP x: sorts the queue in decending order based on dest_ip_addr
APPENDREAR generator_id: is similar to ADDREAR except for the following changes. First, unlike ADDREAR, do NOT check if the destination IP address is already found in the unsorted                          list. Simply add the new alert record to the back of the queue. This implies that there CAN be duplicate generator_id and dest_ip_addr’s. Second, both the                          generator_id and dest_ip_addr are given on the command line. The remaining parameters in the alert _t structure are all set to zero by default
PRINTQ: like PRINT output the same but for unsorted list (Queue)
QUIT: frees all the dynamic memory and ends the program
