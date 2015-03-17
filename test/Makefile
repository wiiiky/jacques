

all:test-net

test-net:test-net.c
	gcc test-net.c -g `pkg-config --libs --cflags jlib-0.1 jio-0.1` ../src/net.c -o test-net


clean:
	rm -f test-net
