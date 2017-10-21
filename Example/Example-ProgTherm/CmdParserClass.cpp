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
   WriteErrMsg("");
   Cmd=-1;
   erFlag=false;
   Nfield=-1;
   if (inCommand[inCommand.length()-1] == '.' && inCommand[0]=='#')//
    {
       //unsigned short value;
       char data[100];
       Log.Verbose("\nCMD =");Log.Verbose(String(inCommand));
       Nfield=(uint8_t)stringParser(inCommand);
       Cmd=(uint8_t)Field[Fcmd];  
       //  Serial.print("Cmd=");Serial.println(Cmd);
       Log.Verbose("\nerrMsg="); Log.Verbose(String(errMsg));
       Log.Verbose("\nNfield="); Log.Verbose(String(Nfield,DEC));Log.Verbose("\n");
    }  
    else
    {
      erFlag=true;
      WriteErrMsg("Sintax error");
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
    
    if (isDigit(s[is])) {
      // convert the incoming byte to a char
      // and add it to the string:
      inString += (char)s[is];
    }
    if(ncount<MAX_CMD_F)
    {      
      if (s[is] == ',' ) 
      {
       Field[ncount]=inString.toInt();
      Log.Verbose("\n>");Log.Verbose(String(Field[ncount]));
       ncount++;
       // clear the string for new input:
       inString = "";
      }
      else if(s[is] == '.' )
      {
       Field[ncount]=inString.toInt();
       inString = "";
       erFlag=false;
       WriteErrMsg("No error");
       Log.Verbose("\n>");Log.Verbose(String(Field[ncount]));
      //Serial.print("\r\nCRC=");Serial.println(Field[ncount]);
     // Serial.print('>');Serial.println(Field[ncount]);
      ncount++;
     }
    }
    else
    {
      erFlag=true;
      WriteErrMsg("N field exceeds "+MAX_CMD_F);

    }
  }//end for
  inString = "";
  return ncount;
}
void CmdParserClass::WriteErrMsg(String s)
{
  int l=s.length();
  Log.Verbose("Error:");Log.Verbose(s);Log.Verbose("\tlength=");Log.Verbose(String(l,DEC));Log.Verbose("\n");
  if(l<=MAX_ERR_MSG) s.toCharArray(errMsg,l);
  return;
}


