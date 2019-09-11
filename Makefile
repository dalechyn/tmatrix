all:
	$(CC) -o tmatrix tmatrix.c

install:
	install tmatrix $(DESTDIR)/bin
	install tmatrix.1 $(DESTDIR)/share/man/man1

clean:
	rm -f tmatrix
