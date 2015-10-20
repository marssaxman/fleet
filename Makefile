
all:
	cd startc && $(MAKE)
	cd core && $(MAKE)
	cd runc && $(MAKE)

clean:
	cd startc && $(MAKE) clean
	cd core && $(MAKE) clean
	cd runc && $(MAKE) clean

