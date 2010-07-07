all: index.o counter.o template.o
	gcc -o index index.o counter.o template.o

clean:
	rm *.o

index.o: index.c
	gcc -c index.c

counter.o: counter.c counter.h
	gcc -c counter.c

template.o: template.c template.h
	gcc -c template.c
