CC = gcc
CFLAGS = -Wall

all: demo libcex.a

libcex.a:
	$(CC) $(CFLAGS) -c cex.c
	ar rcs libcex.a cex.o
	
demo: libcex.a
	$(CC) $(CFLAGS) demo.c libcex.a -o demo
	
clean:
	-rm libcex.a demo
