#ifndef LOCALSTORAGE_H
#define LOCALSTORAGE_H

#include <FS.h>

class LocalStorageClass {
    public:
        void begin();
        void handle();
        bool exists(const char*);
        File open(const char*, const char*);
        File open(const String&, const char*);
        bool remove(const char*);
        bool remove(const String&);
        bool rename(const char* pathFrom, const char* pathTo);
        bool rename(const char* pathFrom, const String& pathTo);
        Dir openDir(const char* path);
        Dir openDir(const String& path);
    protected:
};

extern LocalStorageClass LocalStorage;

#endif
