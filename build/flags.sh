# A crosscompiler might make our lives easier, but in the meantime we'll just
# tell GCC not to use any of the services intended for the host platform.
CCFLAGS="$CCFLAGS -ffreestanding -nostdlib -nostdinc"
LDFLAGS="$LDFLAGS -static -nostdlib -static-libgcc"
LIBRARIES="$LIBRARIES -lgcc"

# Point the way to the C runtime.
CCFLAGS="$CCFLAGS -I $FLEETBASE/crt/include"
LDFLAGS="$LDFLAGS -L $FLEETBASE/crt/"
LIBRARIES="$LIBRARIES -lcrt"

# Add the relevant include/link flags for all libraries this build will use.
for lib in $LIBS; do
	CCFLAGS="$CCFLAGS -I $LIBDIR/$lib/include"
	LDFLAGS="$LDFLAGS -L $LIBDIR/$lib"
	LIBRARIES="$LIBRARIES -l$lib"
done


