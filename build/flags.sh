# A crosscompiler might make our lives easier, but in the meantime we'll just
# tell GCC not to use any of the services intended for the host platform.
CCFLAGS="$CCFLAGS -ffreestanding -nostdlib -nostdinc"
LDFLAGS="$LDFLAGS -static -nostdlib -static-libgcc -lgcc"

# Add the relevant include/link flags for all libraries this build will use.
for lib in $LIBS
do
	CCFLAGS="$CCFLAGS -I $LIBDIR/$lib/include"
	LDFLAGS="$LDFLAGS -l$lib -L $LIBDIR/$lib"
done


