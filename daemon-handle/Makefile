all:
	gcc -o build/handle handle.c -lpthread
	gcc -o build/daemon daemon.c -lpthread

handle:
	gcc -o build/handle handle.c -lpthread

daemon:
	gcc -o build/daemon daemon.c -lpthread

clean:
	rm -f logs.txt build/handle build/daemon