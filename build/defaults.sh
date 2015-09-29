# Root directory for the fleet system:
: ${FLEETBASE=~/fleet/}
# Path to the system library directory:
: ${LIBDIR=$FLEETBASE/lib}
# List of system libraries to link against:
: ${LIBS=}

# Name of program to build:
: ${NAME=out}
# Directory for .o and .d files:
: ${OBJDIR=./obj}
# Source tree location:
: ${SRCDIR=./src}
# Compiler to invoke:
: ${CPP=gcc}
# Extra flags to pass the compiler on each source file:
: ${CCFLAGS=}
# Extra flags to pass the compiler when linking:
: ${LDFLAGS=}
# Standard to use when compiling .c files:
: ${STDC=c99}
# Standard to use when compiling .cpp files:
: ${STDCPP=c++11}

