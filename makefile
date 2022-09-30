all:
	make -C consola
	make -C kernel

clean:
	make clean -C consola
	make clean -C kernel

run-consola:
	make valgrind -C consola

run-kernel:
	make valgrind -C kernel

debug-consola:
	make debug -C consola