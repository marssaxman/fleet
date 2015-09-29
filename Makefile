
all:
	cd crt; make
	cd lib; make
	cd app; make

clean:
	cd crt; make clean
	cd lib; make clean
	cd app; make clean

