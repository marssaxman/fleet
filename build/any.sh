# We've been configured with environment variables describing a project to
# build. See what's out of date and compile anything that has been changed.
echo "compiling $NAME"
BUILDDIR=`dirname $0`
# Set default values for the environment variables that configure this script.
source $BUILDDIR/defaults.sh
# Add compiler & linker flags for library access and platform compatibility.
source $BUILDDIR/flags.sh
# Delete any object files whose dependencies have changed since the last build.
source $BUILDDIR/checkdeps.sh
# Compile everything that doesn't have a current object file.
source $BUILDDIR/compile.sh

