#ifndef CRASHTOFS_H
#define CRASHTOFS_H

#include "Arduino.h"
#include "user_interface.h"

#define CRASHFILEPATH "/www/crash.txt"

class CrashToFSClass
{
  public:
  	CrashToFSClass(char *otherfilename=0);
    void clearFile();
};

void savetoFS(struct rst_info * rst_info, uint32_t stack, uint32_t stack_end, Print& outputDev );

extern CrashToFSClass CrashToFS;

#endif
