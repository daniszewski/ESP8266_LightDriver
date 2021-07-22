#include "CrashToFS.h"
#include <LittleFS.h>

char *staticfn=0;

void savetoFS(struct rst_info * rst_info, uint32_t stack, uint32_t stack_end,Print& outputDev ){
	outputDev.printf(PSTR("Crash # at %ld ms, UTC %ld \n"), millis(), time(nullptr));
	outputDev.printf(PSTR("Reason of restart: %d\n"), rst_info->reason);
	outputDev.printf(PSTR("Exception cause: %d\n"), rst_info->exccause);
	outputDev.printf(PSTR("epc1=0x%08x epc2=0x%08x epc3=0x%08x excvaddr=0x%08x depc=0x%08x\n"), rst_info->epc1, rst_info->epc2, rst_info->epc3, rst_info->excvaddr, rst_info->depc);
	outputDev.println(PSTR(">>>stack>>>"));
	int16_t stackLength = stack_end - stack;
	uint32_t stackTrace;
	// write stack trace to EEPROM
	for (int16_t i = 0; i < stackLength; i += 0x10)
	{
		outputDev.printf(PSTR("%08x: "), stack + i);
		for (byte j = 0; j < 4; j++)
		{
			uint32_t* byteptr = (uint32_t*) (stack + i+j*4);
			stackTrace=*byteptr;
			outputDev.printf(PSTR("%08x "), stackTrace);
		}
		outputDev.println();
	}
	outputDev.println(PSTR("<<<stack<<<\n"));
}

extern "C" void custom_crash_callback(struct rst_info * rst_info, uint32_t stack, uint32_t stack_end )
{
	class StringPrinter2 : public Print {
	public:
		String str="";
		StringPrinter2(){};
		virtual size_t write(const uint8_t character){str+=character;return 1;};
		virtual size_t write(const uint8_t *buffer, size_t size){
			String str2=String((const char *)buffer);
			str2.remove(size);
			str+=str2;
			return size;
		};
	} strprinter2;
	savetoFS(rst_info, stack, stack_end,strprinter2);

	String fn;
	if(staticfn) fn=String(staticfn);
	else fn=String(F(CRASHFILEPATH));
	File f = LittleFS.open(fn, "a");
	if(!f) f= LittleFS.open(fn, "w");
	if(f) {
		f.write((uint8_t*)strprinter2.str.c_str(), strprinter2.str.length());
		f.close();
	}
}

CrashToFSClass::CrashToFSClass(char *otherfilename) {
	staticfn=otherfilename;
}

void CrashToFSClass::clearFile(void)
{
	String fn;
	if(staticfn) fn=String(staticfn);
	else fn=String(F(CRASHFILEPATH));
	LittleFS.remove(fn);
}

CrashToFSClass CrashToFS;
