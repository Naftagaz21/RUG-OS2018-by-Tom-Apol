CC     = gcc
CFLAGS = -std=c99 -Wall -g
DUPLICATES_HEADER = DynamicArray.h

execute:    execute.c
						$(CC) $(CFLAGS) execute.c -o $@

ring:       ring.c
						$(CC) $(CFLAGS) ring.c -o $@

duplicates: duplicates.c DynamicArray.c $(DUPLICATES_HEADER)
						$(CC) $(CFLAGS) duplicates.c DynamicArray.c -o $@
