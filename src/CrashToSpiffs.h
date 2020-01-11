#ifndef CRASHSPIFFS_H
#define CRASHSPIFFS_H

#include "Arduino.h"
#include "user_interface.h"

#define CRASHFILEPATH "/www/crash.txt"

class CrashToSpiffsClass
{
  public:
  	CrashToSpiffsClass(char *otherfilename=0);
    void clearFile();
};

void savetoSpiffs(struct rst_info * rst_info, uint32_t stack, uint32_t stack_end, Print& outputDev );

extern CrashToSpiffsClass SaveCrashSpiffs;

#endif
