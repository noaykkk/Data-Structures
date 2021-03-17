# makefile 
#
# -lm is used to link in the math library
# -Wall turns on all warning messages 
#
comp = gcc
comp_flags = -g -Wall 
comp_libs = -lm  

test : table.o test.o
	$(comp) $(comp_flags)  table.o lab7.o -o lab7 $(comp_libs)

table.o : table.c table.h
	$(comp) $(comp_flags) -c table.c

test.o : main.c table.h
	$(comp) $(comp_flags) -c main.c

clean :
	rm -f *.o test core

