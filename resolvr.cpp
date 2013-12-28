#include <iostream>
#include <string>
#include <list>
#include <sstream>

#include <stdlib.h>

#define VERSION "1.0.0"

std::string path;
std::string base;
std::string prog;

// Windows compilers will normally set this, otherwise Windows compilation can
// be forced by explicitly setting -DOS_WIN
#ifdef _WIN32
#define OS_WIN
#endif

#define RESET   "\033[0m"
#define BOLD    "\033[1m"

#ifdef OS_WIN

// Use implementation of realpath based on GetFullPathNameA by Nach M. S.
#include "realpath.h"

#define SEPARATOR '\\'
#define VOL_SEP ':'

#else

#define SEPARATOR '/'
#define VOL_SEP '\0'

#endif

bool resolveSymlinks = false;
bool isAbsolute = false;

enum HelpLevel {
    HelpLevelShort,
    HelpLevelLong
};

bool isRelative(const std::string &path)
{
    return (path.at(0) != SEPARATOR);
}

std::string volumePart(const std::string &path)
{
    std::string result;
    if (VOL_SEP && path.at(1) == VOL_SEP)
    {
        result = path.substr(0, 2);
    }
    return result;
}

std::string pathPart(const std::string &path)
{
    std::string result = path;
    if (VOL_SEP && (path.at(1) == VOL_SEP))
    {
        result = path.substr(2);
    }
    return result;
}

void printHelp(HelpLevel helpLevel = HelpLevelShort)
{
    if (helpLevel == HelpLevelShort)
    {
        std::cerr << "Usage: " << prog << " [-h] | [-s] path [relative_path]" << std::endl;
    }
    else
    {
        std::cerr << "\n"
                     BOLD "SYNOPSIS\n" RESET
                     "\n"
                     "       " << prog << " [-h] | [-s] path [relative_path]\n"
                     "\n"
                     BOLD "DESCRIPTION\n" RESET
                     "\n"
                     "    Returns " BOLD "path" RESET " resolved to canonical form, that is with \"..\", \".\" and\n"
                     "    doubled-seperators \"" << SEPARATOR << SEPARATOR << "\" replaced where possible so as to form\n"
                     "    the simplest result that still specifies the same path.\n"
                     "\n"
                     "    In the two-argument form, returns the " BOLD "path" RESET " in the first argument resolved\n"
                     "    as relative to the " BOLD "relative_path" RESET " in the second argument.\n"
                     "\n"
                     "    If the path/s could be resolved resolvr returns EXIT_SUCCESS and outputs the\n"
                     "    resolved path, otherwise it returns EXIT_FAILURE.\n"
                     "\n"
                     BOLD "OPTIONS\n" RESET
                     "\n"
                     BOLD "    -s\n" RESET
                     "        In the default case this program only does string manipulation, and does not\n"
                     "        reference the file system at all.  With -s symlinks are resolved and paths\n"
                     "        are checked on the file system.  See PATH RESOLUTION below.\n"
                     "\n"
                     BOLD "    -h\n" RESET
                     "        Print this help and exit with EXIT_SUCCESS.\n"
                     "\n"
                     BOLD "PLATFORMS\n" RESET
                     "\n"
                     "    If -s is specified then this program will resolve symlinks to their targets, and\n"
                     "    will fail with EXIT_FAILURE if the target path does not exist.  In the case of\n"
                     "    -s the " BOLD "second argument is ignored" RESET " and the current directory is assumed\n"
                     "    as the base for any relative path.\n"
                     "\n"
                     "    On Windows uses \"\\\" as the path separator character, on all other platforms\n"
                     "    the \"/\" character.\n"
                     "\n"
                     BOLD "PATH RESOLUTION\n" RESET
                     "\n"
                     "    In the -s case path resolution is as per the realpath C library function as here:\n"
                     "\n"
                     "      http://pubs.opengroup.org/onlinepubs/9699919799/functions/realpath.html\n"
                     "\n"
                     "    so in the -s case this tool is really just a simple wrapper around realpath (on\n"
                     "    those systems that provide it).\n"
                     "\n"
                     "    Some systems (not Mac OSX or Windows) have command line versions of realpath or an\n"
                     "    equivalent in readlink -f which will return a canonical path.\n"
                     "\n"
                     "    If shell versions of realpath are available this program in the -s case is equivalent\n"
                     "    to the bash shell code:\n"
                     "\n"
                     "        realpath \"$2/$1\"\n"
                     "\n"
                     BOLD "CREDIT\n" RESET
                     "\n"
                     "    Written by Sarah Smith 2013.\n"
                     "\n"
                     "    Windows implementation of \"realpath\" is by Nach M. S. as described here:\n"
                     "\n"
                     "      http://sourceforge.net/p/mingw/patches/256/"
                     << std::endl;
    }
}

int main(int argc, char *argv[])
{
    std::list<std::string> args;
    for (int i = 0; i < argc; ++i)
    {
        args.push_back( std::string( argv[i] ));
    }

    prog = args.front();
    args.pop_front();

    while (args.size() > 0)
    {
        std::string opt = args.front();
        args.pop_front();
        if (opt == "-h")
        {
            printHelp(HelpLevelLong);
            exit( EXIT_SUCCESS );
        }
        else if (opt == "-s")
        {
            resolveSymlinks = true;
        }
        else if (path.size() == 0)
        {
            path = opt;
        }
        else if (base.size() == 0)
        {
            base = opt;
        }
        else
        {
            std::cerr << "Ignoring extra arguments.  (Do you need to quote or escape spaces?)" << std::endl;
            printHelp();
        }
    }
    if (path.size() == 0)
    {
        printHelp();
        exit( EXIT_FAILURE );
    }

    std::string volPart = volumePart(path);
    if (volPart.size() > 0)
        path = pathPart(path);

    if (base.size() > 0)
        path = base + SEPARATOR + path;

    isAbsolute = !isRelative(path);

    std::cerr << "resolving \"" << path << "\"\n";

    if (resolveSymlinks)
    {
        // Note in this case we ignore the relative_path "base" argument
        char *resolved = realpath( path.c_str(), NULL );
        path = resolved;
        free( resolved );
    }
    else
    {
        std::stringstream ss(path);
        std::string item;
        std::list<std::string> pathParts;
        while (std::getline(ss, item, SEPARATOR))
        {
            // drop empty parts (from eg "//") and 'dots' from eg "/./"
            if (item.size() == 0 || item == ".") continue;
            // go up a dir on "/../" parts if not at beginning of a path
            if (item == ".." && pathParts.size() > 0)
            {
                pathParts.pop_back();
                continue;
            }
            // in all other cases accumulate to the resulting path
            pathParts.push_back(item);
        }
        path.clear();
        if (pathParts.size() == 0)
        {
            path = isAbsolute ? SEPARATOR : '.';
        }
        else
        {
            if (isAbsolute)
                path = SEPARATOR;
            path = path + pathParts.front();
            pathParts.pop_front();
            std::list<std::string>::const_iterator it = pathParts.begin();
            for ( ; it != pathParts.end(); ++it)
            {
                path = path + SEPARATOR + *it;
            }
        }
    }

    if (volPart.size() > 0)
        path = volPart + path;

    std::cout << path << std::endl;
}
