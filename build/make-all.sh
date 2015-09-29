#!/usr/bin/env bash
# Look at each subdirectory in the current working directory.
# If it contains a makefile, run it.
# You can specify a target to apply in each submake as arg 1.
for subdir in $(find . -maxdepth 1 -type d ! -iname ".*" ); do
	if [ -e $subdir/Makefile ]; then
		pushd $subdir > /dev/null
		make $1
		popd > /dev/null
	fi
done

