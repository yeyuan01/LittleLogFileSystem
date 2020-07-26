CC = gcc
CFALGS = -Wall -g

.PHONY: all
all:
	$(CC) $(CFLAGS) -I./../io/ ./../io/File.c test01.c -o test01
	$(CC) $(CFLAGS) -I./../io/ ./../io/File.c test02.c -o test02
.PHONY: clean
clean:
	rm -rf test01 test02

