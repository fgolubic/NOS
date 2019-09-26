all: philosophers.out first.out pusac.out trgovac.out

philosophers.out: philosophers.o
	gcc -o philosophers.out philosophers.o
philosophers.o: philosophers.c
	gcc -c -Wall philosophers.c

first.out: first.o
	gcc -o first.out first.o
first.o: first.c
	gcc -c -Wall first.c

pusac.out: pusac.o
	gcc -o pusac.out pusac.o
pusac.o: pusac.c
	gcc -c -Wall pusac.c

trgovac.out: trgovac.o
	gcc -o trgovac.out trgovac.o
trgovac.o: trgovac.c
	gcc -c -Wall trgovac.c

clean:
	-rm -f philosophers.out first.out pusac.out trgovac.out *.o *.core
