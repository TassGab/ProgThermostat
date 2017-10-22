#ifndef Logging_h
#define Logging_h
#include <Arduino.h>
#include <chibi.h>
//#include <Crc16.h>
typedef enum {_Verbose,_Debug, _Info, _Error} Log_en;
class LoggingCs
{
  public:
   LoggingCs();
   Log_en LogLevel;
   Log_en ZBLogLevel;
   bool ZBen;
   bool AutoCR;
   void Verbose(String);
   void Debug(String);
   void Info(String);
   void Error(String);
  private:
   void Print(String);
   void SendZB(String);
   byte CRC8(const byte *, byte);    
};
extern LoggingCs Log; //make an instance for the user

#endif
