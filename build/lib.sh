#!/usr/bin/env bash
#
# Build process for freestanding static libraries in the fleet architecture.
#
# Run this from the main target in a makefile.

CCFLAGS="$CCFLAGS -I ./include"
source `dirname $0`/any.sh
objfiles=$(find $OBJDIR -name "*.o")
OUTFILE=lib$NAME.a
ar rcs $OUTFILE $objfiles
ls -l $OUTFILE



