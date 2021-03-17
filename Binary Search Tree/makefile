# makefile
# Version: 1
#
# -lm is used to link in the math library
#
# -Wall turns on all warning messages 
#
comp = gcc
comp_flags = -g -Wall 
comp_libs = -lm  

test : tree.o test.o
	$(comp) $(comp_flags) tree.o test.o -o test $(comp_libs)

tree.o : tree.h
	$(comp) $(comp_flags) -c tree.c

test.o : main.c tree.h
	$(comp) $(comp_flags) -c main.c

clean :
	rm -f *.o test core

