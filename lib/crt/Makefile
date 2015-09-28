all: library

library:
	@as src/start.s -o obj/start.o
	@ar rcs libcrt.a obj/start.o
	@ls -l libcrt.a

clean:
	@rm -rf libcrt.a obj/start.o

