#!/bin/sh
# Copyright 2022 Collabora Ltd.
# SPDX-License-Identifier: Zlib

set -eu

cd `dirname $0`/..

ref_major=$(sed -ne 's/^#define SDL_MAJOR_VERSION  *//p' include/SDL/SDL_version.h)
ref_minor=$(sed -ne 's/^#define SDL_MINOR_VERSION  *//p' include/SDL/SDL_version.h)
ref_micro=$(sed -ne 's/^#define SDL_PATCHLEVEL  *//p' include/SDL/SDL_version.h)
ref_version="${ref_major}.${ref_minor}.${ref_micro}"

tests=0
failed=0

ok () {
    tests=$(( tests + 1 ))
    echo "ok - $*"
}

not_ok () {
    tests=$(( tests + 1 ))
    echo "not ok - $*"
    failed=1
}

version=$(sed -Ene 's/^project\(sdl[0-9]+_compat VERSION ([0-9.]*) LANGUAGES C\)$/\1/p' CMakeLists.txt)

if [ "$ref_version" = "$version" ]; then
    ok "CMakeLists.txt $version"
else
    not_ok "CMakeLists.txt $version disagrees with SDL_version.h $ref_version"
fi

tuple=$(sed -ne 's/^ *FILEVERSION *//p' src/version.rc | tr -d '\r')
ref_tuple="${ref_major},${ref_minor},${ref_micro},0"

if [ "$ref_tuple" = "$tuple" ]; then
    ok "version.rc FILEVERSION $tuple"
else
    not_ok "version.rc FILEVERSION $tuple disagrees with SDL_version.h $ref_tuple"
fi

tuple=$(sed -ne 's/^ *PRODUCTVERSION *//p' src/version.rc | tr -d '\r')

if [ "$ref_tuple" = "$tuple" ]; then
    ok "version.rc PRODUCTVERSION $tuple"
else
    not_ok "version.rc PRODUCTVERSION $tuple disagrees with SDL_version.h $ref_tuple"
fi

tuple=$(sed -Ene 's/^ *VALUE "FileVersion", "([0-9, ]*)\\0"\r?$/\1/p' src/version.rc | tr -d '\r')
ref_tuple="${ref_major}, ${ref_minor}, ${ref_micro}, 0"

if [ "$ref_tuple" = "$tuple" ]; then
    ok "version.rc FileVersion $tuple"
else
    not_ok "version.rc FileVersion $tuple disagrees with SDL_version.h $ref_tuple"
fi

tuple=$(sed -Ene 's/^ *VALUE "ProductVersion", "([0-9, ]*)\\0"\r?$/\1/p' src/version.rc | tr -d '\r')

if [ "$ref_tuple" = "$tuple" ]; then
    ok "version.rc ProductVersion $tuple"
else
    not_ok "version.rc ProductVersion $tuple disagrees with SDL_version.h $ref_tuple"
fi

micro=$(sed -Ene 's/^#define SDL12_COMPAT_VERSION ([0-9]+)$/\1/p' src/SDL12_compat.c)

if [ "$ref_micro" = "$micro" ]; then
    ok "SDL12_compat.c SDL12_COMPAT_VERSION $micro"
else
    not_ok "SDL12_compat.c SDL12_COMPAT_VERSION $micro disagrees with SDL_version.h $ref_micro"
fi

so_version="1.2.$ref_micro"
compat_version="1.0"
dylib_version="12.$ref_micro"

ref_dylib_versions="$compat_version $dylib_version"
dylib_versions=$(sed -Ene 's/^LDFLAGS\+= -Wl,-compatibility_version,([0-9.]+) -Wl,-current_version,([0-9.]+)$/\1 \2/p' src/Makefile.darwin)

if [ "$ref_dylib_versions" = "$dylib_versions" ]; then
    ok "Makefile.darwin LDFLAGS $dylib_versions"
else
    not_ok "Makefile.darwin LDFLAGS $dylib_versions disagrees with reference $ref_dylib_versions"
fi

ref_so_version="$so_version"
so_version=$(sed -Ene 's/^SHLIB = libSDL-1.2.so.([0-9.]+)$/\1/p' src/Makefile.linux)

if [ "$ref_so_version" = "$so_version" ]; then
    ok "Makefile.linux SHLIB $so_version"
else
    not_ok "Makefile.linux SHLIB $so_version disagrees with reference $ref_so_version"
fi

echo "1..$tests"
exit "$failed"
