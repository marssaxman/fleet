# Run the main compile loop.
# Examine each source file. If there is not already a .o file for it in the
# objdir, then we must recompile it. This will happen the first time we
# compile, every time we change the source file, or every time we change one of
# the source file's dependencies.
errorfound=0
srcfiles=$(find $SRCDIR -type f -name "*.cpp" -o -name "*.c" -o -name "*.s")
for src in $srcfiles; do
	# make up an obj file path in our obj subdirectory
	# Strip off any leading "./" which might happen to be present
	src="${src#\./}"
	extension="${src##*.}"
	relpath="${src%/*.*}"
	mkdir -p "$OBJDIR/$relpath"
	objname="${src%.*}.o"
	obj="$OBJDIR/$objname"
	depname="${src%.*}.d"
	dep="$OBJDIR/$depname"

	# Do we need to recompile the source file?
	if [[ -e $obj && -e $dep ]] ; then
		# an object file exists, so no need to recompile
		# echo "using $objname"
		true
	else
		# no object file, so we'll compile one
		echo "compiling $src..."

		# only specify C99 if we are not compiling a c++ file
		if [[ $extension == "c" ]]; then
			stdarg="-std=$STDC"
		elif [[ $extension == "cpp" ]]; then
			stdarg="-std=$STDCPP"
		fi

		if $CPP $CCFLAGS -MD $stdarg -I$SRCDIR -c $src -o $obj
		then
			echo -n ""
		else
			errorfound=1
		fi
	fi
done

