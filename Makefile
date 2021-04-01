pctest : pctest.o
	gcc -o pctest pctest.o

pctest.o : pctest.c
	gcc -c pctest.c
