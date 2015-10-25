CC=gcc


libjac.so: socket.c
	$(CC) $^ -I../libev -shared -fPIC -o $@

all: libjac.so

clean:
	-rm -f *.so

distclean: clean
