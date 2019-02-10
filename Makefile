default: tm

debug:
	gcc -g tmatrix.c -o dtm

tmatrix.o : tmatrix.c
	gcc tmatrix.c -c

tm : tmatrix.o
	gcc tmatrix.o -o tm

clean:
	rm -f tm
	rm -f tmatrix.o
