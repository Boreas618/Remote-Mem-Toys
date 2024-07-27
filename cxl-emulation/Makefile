CC=gcc
CFLAGS=-Wall -I/usr/local/include
LDFLAGS=-lmemkind -lnuma

emulate: emulate.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: emulate
	export LD_LIBRARY_PATH=/usr/local/lib:$$LD_LIBRARY_PATH; \
	MEMKIND_DAX_KMEM_NODES=1 numactl --membind=0 --cpunodebind=0 ./emulate

clean:
	rm -f emulate