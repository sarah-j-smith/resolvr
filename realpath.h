#ifndef __REALPATH__
#define __REALPATH__

extern "C" {
    char *realpath(const char *path, char resolved_path[]);
}

#endif // __REALPATH__
