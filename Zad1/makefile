all: pusac.out trgovac.out

pusac.out: pusac.o
	gcc -o pusac.out pusac.o
pusac.o: pusac.c
	gcc -c -Wall pusac.c

trgovac.out: trgovac.o
	gcc -o trgovac.out trgovac.o
trgovac.o: trgovac.c
	gcc -c -Wall trgovac.c

clean:
	-rm -f pusac.out trgovac.out *.o *.core
