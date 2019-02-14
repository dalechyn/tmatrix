default: tmatrix

debug:
	gcc -g tmatrix.c -o dtm

tmatrix.o : tmatrix.c
	gcc tmatrix.c -c

tmatrix : tmatrix.o
	gcc tmatrix.o -o tmatrix

clean:
	rm -f tmatrix
	rm -f tmatrix.o
