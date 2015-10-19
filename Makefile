
all:
	cd startc; make
	cd core; make
	cd app; make

clean:
	cd startc; make clean
	cd core; make clean
	cd app; make clean

