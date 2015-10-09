

CC = gcc

CFLAGS = `pkg-config --cflags jmod-0.1`
LDFLAGS = `pkg-config --libs jmod-0.1`

test: test.c
	$(CC) test.c -shared -fPIC -o test.so $(CFLAGS) $(LDFLAGS)

all: test

clean:
	-rm -f *.so


distclean: clean
