# Name of executable boot image to build:
: ${NAME=executable}
# Directory for .o and .d files:
: ${OBJDIR=./obj}
# Source tree location:
: ${SRCDIR=./src}
# Path to the system library directory
: ${LIBDIR=../lib}
# List of system libraries to link against
: ${LIBS=}
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

