# A crosscompiler might make our lives easier, but in the meantime we'll just
# tell GCC not to use any of the services intended for the host platform.
CCFLAGS="$CCFLAGS -ffreestanding -nostdlib -nostdinc -m32"
LDFLAGS="$LDFLAGS -static -nostdlib -static-libgcc -m32"
LIBRARIES="$LIBRARIES -lgcc"

# Point the way to the C runtime library.
CCFLAGS="$CCFLAGS -I $FLEETBASE/startc/include"
LDFLAGS="$LDFLAGS -L $FLEETBASE/startc/"
LIBRARIES="$LIBRARIES -lstartc"

# Won't get very far without the core library either.
CCFLAGS="$CCFLAGS -I $FLEETBASE/core/include"
LDFLAGS="$LDFLAGS -L $FLEETBASE/core/"
LIBRARIES="$LIBRARIES -lcore"

# Add the relevant include/link flags for all libraries this build will use.
for lib in $LIBS; do
	CCFLAGS="$CCFLAGS -I $LIBDIR/$lib/include"
	LDFLAGS="$LDFLAGS -L $LIBDIR/$lib"
	LIBRARIES="$LIBRARIES -l$lib"
done


