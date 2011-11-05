CC = clang

binarydump: binarydump.o
	$(CC) -o binarydump binarydump.o

binarydump.o: binarydump.c
	$(CC) -c binarydump.c

clean:
	rm -i *.o
	rm -i binarydump