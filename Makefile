crawler: main.o http.o reghelp.o queue.o urls.o memory.o
	gcc main.o http.o reghelp.o queue.o urls.o memory.o -o crawler

main.o: main.c
	gcc -c main.c

http.o: http.c
	gcc -c http.c

urls.o: urls.c
	gcc -c urls.c

queue.o : queue.c
	gcc -c queue.c

reghelp.o: reghelp.c
	gcc -c reghelp.c

memory.o: memory.c
	gcc -c memory.c

clean:
	rm main.o http.o queue.o reghelp.o urls.o crawler

