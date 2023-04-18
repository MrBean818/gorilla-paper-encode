test: test.c gorilla.c gorilla.h
	gcc -g gorilla.c test.c -o test

clean:
	rm -f *.o
	rm -f test

