# The makefile 
#
# make          -- to build program 
# make design   -- to verify no illegal variables
# make driver   -- to compile testing program
# make clean    -- to delete object files, executable, and core
# make llist.o  -- to compile only llist.o
#
#
# Format for each entry
#    target : dependency list of targets or files
#    <tab> command 1
#    <tab> command 2
#    ...
#    <tab> last command
#    <blank line>   -- the list of commands must end with a blank line

test : llist.o ids_support.o test.o
	gcc -Wall -g llist.o ids_support.o test.o -o test

llist.o : llist.c datatypes.h llist.h
	gcc -Wall -g -c llist.c

ids_support.o : ids_support.c datatypes.h llist.h ids_support.h
	gcc -Wall -g -c ids_support.c

test.o : main.c datatypes.h llist.h ids_support.h
	gcc -Wall -g -c test.c

driver : driver.o llist.o ids_support.o
	gcc -Wall -g llist.o ids_support.o driver.o -o driver

driver.o : driver.c datatypes.h llist.h ids_support.h
	gcc -Wall -g -c driver.c

#  @ prefix suppresses output of shell command
#  - prefix ignore errors
#  @command || true to avoid Make's error
#  : is shorthand for true
design :
	@grep -e "-> *ll_front" lab.c ids_support.c ||:
	@grep -e "-> *ll_back" lab.c ids_support.c ||:
	@grep -e "-> *ll_entry_count" lab.c ids_support.c ||:
	@grep -e "-> *ll_sorted_state" lab.c ids_support.c ||:
	@grep -e "-> *ll_next" lab.c ids_support.c ||:
	@grep -e "-> *ll_prev" lab.c ids_support.c ||:
	@grep -e "-> *data_ptr" lab.c ids_support.c ||:
	@grep "compare_fun" lab.c ids_support.c ||:
	@grep "llist_elem_t" lab.c ids_support.c ||:
	@grep "generator_id" llist.c ||:
	@grep "dest_ip_addr" llist.c ||:
	@grep "alert_t" llist.c ||:
	@grep "ids_" llist.c ||:

clean :
	rm -f *.o lab driver core a.out

