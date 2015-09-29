# Run the main compile loop.
BUILDDIR=`dirname $0`
# Set default values for the environment variables that configure this script.
source $BUILDDIR/defaults.sh
# Add compiler & linker flags for library access and platform compatibility.
source $BUILDDIR/flags.sh

if  [ -n "$LIBS" ]; then
	# Make sure all of the libraries are up to date.
	echo "checking libraries..."
	for lib in $LIBS; do
		(
			pushd $LIBDIR/$lib > /dev/null
			if !(make 2>&1 > /dev/null) ; then
				echo "Fail: could not make $lib"
				exit 1
			fi
			popd > /dev/null
		)
	done
fi

# Process dependencies: delete any obj files that are out of date. Each
# dependency file lists one obj file and a string of file paths it depends on.
# If any of those dependencies are newer than the obj file, we'll delete it,
# which will cause it to be recompiled on the next loop.
echo "checking dependencies..."
mkdir -p $OBJDIR
for dep in $(find $OBJDIR -type f -name "*.d"); do
	depfiledata=$(cat $dep)
	objtarget=${depfiledata%%:*}
	if [[ -e $objtarget ]]
	then
		deplist=${depfiledata##*:}
		for checkfile in $deplist; do
			# Ignore the backslash line-continuation entries gcc puts in the
			# dep file
			if [ "$checkfile" = "\\" ] ; then
				continue
			fi
			# If the obj file depends on a file which is newer, delete it.
			# If the obj file depends on a file which does not exist, that is
			# also a good reason to delete it.
			if [ $checkfile -nt $objtarget -o ! -e $checkfile ] ; then
				rm $objtarget
				break
			fi
		done
	fi
done

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

