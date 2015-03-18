
FLAGS = `pkg-config --libs --cflags jlib-0.1 jio-0.1`

all:test-net test-jac

test-net:test-net.c
	gcc test-net.c -g $(FLAGS) ../src/net.c -o test-net

test-jac:test-jac.c
	gcc test-jac.c $(FLAGS) ../src/net.c -g -o test-jac


clean:
	rm -f test-net test-jac
