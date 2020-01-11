#include "CrashToSpiffs.h"
#include <FS.H>

char *staticfn=0;

void savetoSpiffs(struct rst_info * rst_info, uint32_t stack, uint32_t stack_end,Print& outputDev ){
	uint32_t crashTime = millis();
	outputDev.printf(PSTR("Crash # at %ld ms\n"),crashTime);
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
		virtual size_t write(const uint8_t character){str+=character;};
		virtual size_t write(const uint8_t *buffer, size_t size){
			String str2=String((const char *)buffer);
			str2.remove(size);
			str+=str2;
		};
	} strprinter2;
	savetoSpiffs(rst_info, stack, stack_end,strprinter2);

	String fn;
	if(staticfn) fn=String(staticfn);
	else fn=String(F(CRASHFILEPATH));
	File f = SPIFFS.open(fn, "a");
	if(!f) f= SPIFFS.open(fn, "w");
	if(f) {
		unsigned int w=f.write((uint8_t*)strprinter2.str.c_str(), strprinter2.str.length());
		f.close();
	}
	Serial.println(String()+PSTR("Trace saved to file : ")+fn+"\n");
}

CrashToSpiffsClass::CrashToSpiffsClass(char *otherfilename) {
	staticfn=otherfilename;
}

void CrashToSpiffsClass::clearFile(void)
{
	String fn;
	if(staticfn) fn=String(staticfn);
	else fn=String(F(CRASHFILEPATH));
	SPIFFS.remove(fn);
}

CrashToSpiffsClass CrashToSpiffs;
