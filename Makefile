all:
	$(CC) -o tmatrix tmatrix.c

install:
	mkdir -p $(DESTDIR)/bin
	mkdir -p $(DESTDIR)/share/man/man1
	install tmatrix $(DESTDIR)/bin/tmatrix
	install tmatrix.1 $(DESTDIR)/share/man/man1/tmatrix.1

clean:
	rm -f tmatrix
