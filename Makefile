default: tm

debug:
	gcc -g tmatrix.c -o dtm

install:
	apt install libncursesw5-dev

tmatrix.o : tmatrix.c
	gcc tmatrix.c -c

tm : tmatrix.o
	gcc tmatrix.o -o tm

clean:
	rm -f tm
	rm -f tmatrix.o
