DIREXNAME=msdosdir
EXTREXNAME=msdosextr

all: ${DIREXNAME} ${EXTREXNAME}

${DIREXNAME}: ${DIREXNAME}.c fat12.o
	gcc -o $@ $^

${EXTREXNAME}: ${EXTREXNAME}.c fat12.o
	gcc -o $@ $^

fat12.o: fat12.c
	gcc -c $^ -o $@
