#!/usr/bin/env bash
#
# Build process for fleet programs compiled with GCC.
#
# Find all the source files, check for changes, resolve dependencies, and
# recompile anything that needs an update. If we succeeded, link it together.
# This would generally be invoked from the main build target in a makefile.

source `dirname $0`/any.sh

# If we successfully compiled all of the source files, link the executable.
LDFLAGS="$LDFLAGS -T ../build/linker.ld"
BINFILE=$NAME.bin
if (( !errorfound )) ; then
	echo "linking..."
	# use gcc to link an executable
	if $CPP -o $BINFILE $(find $OBJDIR -name "*.o") $LDFLAGS; then
		echo "done."
	else
		errorfound=1
	fi
fi

if (( !errorfound )) ; then
	ls -l $BINFILE
fi

exit $errorfound
