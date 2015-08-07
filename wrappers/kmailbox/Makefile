all: kmailex

kmailex: kmailbox_usr.o kmailbox_tb.c
	gcc -o kmailex $^

kmailbox_usr.o: kmailbox_usr.c
	gcc -c kmailbox_usr.c -o kmailbox_usr.o
