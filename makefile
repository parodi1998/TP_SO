all:
	make -C consola
	make -C kernel
	make -C cpu

clean:
	make clean -C consola
	make clean -C kernel
	make clean -C kernel

run-consola:
	make valgrind -C consola

run-kernel:
	make valgrind -C kernel

run-cpu:
	make valgrind -C cpu

debug-consola:
	make debug -C consola