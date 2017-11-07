/*
 CmdParserClass.cpp - Library for Parsing commands
 Gabriele Tasselli, February 13, 2017
 Command format: #CMD,Nfield,f1,f2,..fn.
 in case CRC is added after the final "."
 */
//#include <Crc16.h>
#include "CmdParserClass.h"
#define Fcmd 0
#define Ff1 1
void CmdParserClass::Parse(String inCommand){
  //Crc16 crc;
//   WriteErrMsg("");
   Cmd=-1;
   erFlag=false;
   Nfield=-1;
   byte len=byte(inCommand.length());
   if (inCommand[len-2] == '.' && inCommand[0]=='#' && inCommand[len-1] == '\n')//
    {
       //unsigned short value;
       char data[100];
       Log.Debug(F("\nCMD ="));Log.Debug(String(inCommand));
       Nfield=(uint8_t)stringParser(inCommand);
       Cmd=(uint8_t)Field[Fcmd];  
       Log.Verbose(F("\nNfield=")); Log.Verbose(String(Nfield,DEC));Log.Verbose(F("\n"));
    }  
    else
    {
      erFlag=true;
     Log.Error(F("\nCmd Sintax error \n"));
    }
    //inCommand="";
    return;
}
uint8_t CmdParserClass::stringParser(String s)
{
 //Format #CMD,f1,f2,...fn.
 
  uint8_t is=0;
  uint8_t ncount=0;
  //int inChar;
  String inString="";
  for (is=1;is<s.length();is++) //from 1 discard #
  {
    
    if (isDigit(s[is]) || s[is]=='-') {
      // convert the incoming byte to a char
      // and add it to the string:
      inString += (char)s[is];
    }
    if(ncount<MAX_CMD_F)
    {      
      if (s[is] == ',' ) 
      {
       Field[ncount]=inString.toInt();
       Log.Verbose(F("\n>"));Log.Verbose(String(Field[ncount]));Log.Verbose(F("\n"));
       ncount++;
       // clear the string for new input:
       inString = "";
      }
      else if(s[is] == '.' )
      {
       Field[ncount]=inString.toInt();
       inString = "";
       erFlag=false;
       Log.Debug(F("Cmd end found"));
       Log.Verbose(F("\n>"));Log.Verbose(String(Field[ncount]));
      //Serial.print("\r\nCRC=");Serial.println(Field[ncount]);
     // Serial.print('>');Serial.println(Field[ncount]);
      ncount++;
     }
    }
    else
    {
      erFlag=true;
      Log.Error(F("\nN field exceeds "));Log.Error(String(MAX_CMD_F));Log.Error(F("\n"));
    }
  }//end for
  inString = "";
  return ncount;
}



