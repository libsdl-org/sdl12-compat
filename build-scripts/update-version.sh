#!/bin/sh

cd `dirname $0`/..

if [ -z $1 ]; then
    echo "USAGE: $0 <new_patch_version>" 1>&2
    exit 1
fi

NEWVERSION="$1"
echo "Updating version to '1.2.$NEWVERSION' ..."

perl -w -pi -e 's/(VERSION 1\.2\.)\d+/${1}'$NEWVERSION'/;' CMakeLists.txt
perl -w -pi -e 's/(DYLIB_CURRENT_VERSION 12\.)\d+/${1}'$NEWVERSION'/;' CMakeLists.txt
perl -w -pi -e 's/(\-current_version,12\.)\d+/${1}'$NEWVERSION'/;' src/Makefile.darwin
perl -w -pi -e 's/(libSDL\-1\.2\.so\.1\.2\.)\d+/${1}'$NEWVERSION'/;' src/Makefile.linux
perl -w -pi -e 's/(VERSION = 1\.2\.)\d+/${1}'$NEWVERSION'/;' src/Makefile.os2
perl -w -pi -e 's/(VERSION = 1\.2\.)\d+/${1}'$NEWVERSION'/;' src/Makefile.w32
perl -w -pi -e 's/(\#define SDL12_COMPAT_VERSION )\d+/${1}'$NEWVERSION'/;' src/SDL12_compat.c
perl -w -pi -e 's/(\#define SDL_PATCHLEVEL )\d+/${1}'$NEWVERSION'/;' include/SDL/SDL_version.h
perl -w -pi -e 's/(FILEVERSION 1,2,)\d+/${1}'$NEWVERSION'/;' src/version.rc
perl -w -pi -e 's/(PRODUCTVERSION 1,2,)\d+/${1}'$NEWVERSION'/;' src/version.rc
perl -w -pi -e 's/(VALUE "FileVersion", "1, 2, )\d+/${1}'$NEWVERSION'/;' src/version.rc
perl -w -pi -e 's/(VALUE "ProductVersion", "1, 2, )\d+/${1}'$NEWVERSION'/;' src/version.rc

echo "All done."
echo "Run 'git diff' and make sure this looks correct before 'git commit'."

exit 0

