all:
	gcc -o build/handle handle.c -lpthread -ljansson
	gcc -o build/daemon daemon.c -lpthread -ljansson

handle:
	gcc -o build/handle handle.c -lpthread -ljansson

daemon:
	gcc -o build/daemon daemon.c -lpthread -ljansson

clean:
	rm -f logs.txt build/handle build/daemon