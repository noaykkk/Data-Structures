# makefile
#
# -Wall turns on all warning messages 
#
comp = gcc
comp_flags = -g -Wall 
comp_libs = -lm 

test : graph.o support.o test.o
	$(comp) $(comp_flags) graph.o support.o test.o -o test $(comp_libs)

graph.o : graph.c graph.h
	$(comp) $(comp_flags) -c graph.c

support.o : support.c support.h graph.h
	$(comp) $(comp_flags) -c support.c

test.o : main.c graph.h support.h
	$(comp) $(comp_flags) -c main.c

clean :
	rm -f *.o test core