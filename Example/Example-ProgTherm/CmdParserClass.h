/*
 CmdParser.h - Library for parsing serial commands
 Command format: #CMD,<f1>,<f2>,..<fn>,CRC.
 Gabriele Tasselli, February 13, 2017
 */
#ifndef CmdParser_h
#define CmdParser_h
//#include <chibi.h>
#define MAX_CMD_F 10
#define MAX_ERR_MSG 20
#include "Logging_class.h"
class CmdParserClass
{
 public:
 //bool CrcEn=true; //flag for crc test
 uint8_t Cmd;//Command
 uint8_t CrcCalc; //Calculated CRC
 uint8_t Field[MAX_CMD_F];//array of fields
 uint8_t Nfield; //N of fields
 //void (*func1)();//pointer to run a function in Main
 //void (*func2)();//second pointer to run a function in Main
 char errMsg[MAX_ERR_MSG]; //error message string
 bool erFlag; //flag of parsing error
 void Parse(String);
 //CmdParserClass();
 private:
 uint8_t stringParser(String);
 void WriteErrMsg(String);
 //void CmdCall();
};
#endif
