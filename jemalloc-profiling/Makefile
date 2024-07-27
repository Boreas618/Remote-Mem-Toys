CC=gcc
CFLAGS=-I`jemalloc-config --includedir`
LDFLAGS=-L`jemalloc-config --libdir` -Wl,-rpath,`jemalloc-config --libdir` -ljemalloc `jemalloc-config --libs`

profiling: profiling.c
	${CC} ${CFLAGS} -o $@ $^ ${LDFLAGS}

run: profiling
	./profiling

run_leak: profiling
	MALLOC_CONF=prof_leak:true,lg_prof_sample:0,prof_final:true ./profiling

clean:
	rm -rf profiling
	rm -rf *.heap