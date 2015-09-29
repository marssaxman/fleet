
all:
	cd lib; make
	cd app; make

clean:
	cd lib; make clean
	cd app; make clean

