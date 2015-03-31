
FLAGS = `pkg-config --libs --cflags jlib-0.1 jio-0.1`

all: test-jac

test-jac:test-jac.c
	gcc test-jac.c $(FLAGS) -g -o test-jac


clean:
	rm -f test-jac
