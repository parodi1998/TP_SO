all:
	make -C consola

clean:
	make clean -C consola

run-consola:
	make valgrind -C consola

debug-consola:
	make debug -C consola