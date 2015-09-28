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

