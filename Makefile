prefix = /usr/local

all:
	$(CC) -o tmatrix tmatrix.c

install:
	install tmatrix $(DESTDIR)$(prefix)/bin
	install tmatrix.1 $(DESTDIR)$(prefix)/share/man/man1

clean:
	rm -f tmatrix
