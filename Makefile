all: index.o counter.o template.o plugins
	gcc -ldl -o index index.o counter.o template.o

clean:
	rm *.o *.so index -f
	rm plugins/*.o plugins/*.so -f

index.o: index.c
	gcc -c index.c

counter.o: counter.c counter.h
	gcc -c counter.c

template.o: template.c template.h
	gcc -c template.c

plugins: plugins/printDate.so

plugins/printDate.so: plugins/printDate.c
	gcc -c -fpic -o plugins/printDate.o plugins/printDate.c
	gcc -shared -lc -o plugins/printDate.so plugins/printDate.o
