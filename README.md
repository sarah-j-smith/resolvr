resolvr
=======

File path resolver cross-platform with no-symlink-resolve default.

Chances are on Linux that realpath or readlink -f will already do what
you need; unless what you need is a simple string path cleanup on the
command-line, without referring to the filesystem or resolving symlinks.

On MacOSX and Windows there is no realpath for bash so resolvr might be
useful there.

resolvr is meant to be useful when you have a script eg
~/src/myproj/scripts/doit.sh and in the script you want to refer to
~/src/myproj/resources/afile.txt - in that case

    RESOURCE_PATH = `resolvr $(dirname $0)/../resources/afile.txt`
    echo "Using resource $RESOURCE_PATH"
    do_something_with( $RESOURCE_PATH )

...will result in a better experience for users of your shell script as
it will print (if the shell script is run from $HOME):
  
    Using resource depot/myproj/resources/afile.txt

and in the case where say resources is symlinked into the project tree
for some reason it will *still* print that (instead of resolving the
symlink) as realpath would.  If you want a resolved symlink then specify
-s and resolvr will use realpath under the hood.

SYNOPSIS
========

       ./resolvr [-h] | [-s] path [relative_path]

DESCRIPTION
===========

    Returns path resolved to canonical form, that is with "..", "." and
    doubled-seperators "//" replaced where possible so as to form
    the simplest result that still specifies the same path.

    In the two-argument form, returns the path in the first argument resolved
    as relative to the relative_path in the second argument.

    If the path/s could be resolved resolvr returns EXIT_SUCCESS and outputs the
    resolved path, otherwise it returns EXIT_FAILURE.

OPTIONS
=======

    -s
        In the default case this program only does string manipulation, and does not
        reference the file system at all.  With -s symlinks are resolved and paths
        are checked on the file system.  See PATH RESOLUTION below.

    -h
        Print this help and exit with EXIT_SUCCESS.

PLATFORMS
=========

    If -s is specified then this program will resolve symlinks to their targets, and
    will fail with EXIT_FAILURE if the target path does not exist.  In the case of
    -s the second argument is ignored and the current directory is assumed
    as the base for any relative path.

    On Windows uses "\" as the path separator character, on all other platforms
    the "/" character.

PATH RESOLUTION
===============

    In the -s case path resolution is as per the realpath C library function as here:

      http://pubs.opengroup.org/onlinepubs/9699919799/functions/realpath.html

    so in the -s case this tool is really just a simple wrapper around realpath (on
    those systems that provide it).

    Some systems (not Mac OSX or Windows) have command line versions of realpath or an
    equivalent in readlink -f which will return a canonical path.

    If shell versions of realpath are available this program in the -s case is equivalent
    to the bash shell code:

        realpath "$2/$1"

CREDIT
======

    Written by Sarah Smith 2013.

    Windows implementation of "realpath" is by Nach M. S. as described here:

      http://sourceforge.net/p/mingw/patches/256/

