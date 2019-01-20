default: tm

install:
	apt install libncursesw5-dev

tmatrix.o : tmatrix.c
	gcc tmatrix.c -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=600 -c

tm : tmatrix.o
	gcc tmatrix.o -lncursesw -ltinfo -ldl -o tm

clean:
	rm -f tm
	rm -f tmatrix.o
