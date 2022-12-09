all:
	make -C consola
	make -C kernel
	make -C cpu
	make -C Memory

clean:
	make clean -C consola
	make clean -C kernel
	make clean -C cpu
	make clean -C Memory

run-consola:
	make valgrind -C consola

run-kernel:
	make valgrind -C kernel

run-cpu:
	make valgrind -C cpu

run-memoria:
	make valgrind -C Memory

debug-consola:
	make debug -C consola