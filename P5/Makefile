EXNAME=fat12ex

all: ${EXNAME}

${EXNAME}: ${EXNAME}.c fat12.o
	gcc -o ${EXNAME} $^
fat12.o: fat12.c
	gcc -c $^ -o $@
