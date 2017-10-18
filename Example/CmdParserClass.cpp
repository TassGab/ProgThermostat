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
//
//void CmdParserClass::CmdCall()
//{
//  switch(Cmd)
//  {
//    case 1: // set time
//      if (Field[1]>1) 
//      {
//        Serial.println("Chiamata puntatore1");
//        if(func1!=NULL) func1();
//      }
//      else
//      {
//        if(func2!=NULL) 
//        {
//          Serial.println("Chiamata puntatore2");
//          func2();
//        }
//      }
//    break;
//    case 2: //
//    
//    break;
//    default:
//      Serial.println("command unknown");
//  }
//}
//CmdParserClass::CmdParserClass()
//{
//  func1=NULL;
//  func2=NULL;
//}
void CmdParserClass::Parse(String inCommand){
  //Crc16 crc;
   WriteErrMsg("");
   Cmd=-1;
   erFlag=false;
   Nfield=-1;
   if (inCommand[inCommand.length()-1] == '.' && inCommand[0]=='#')//
    {   

       int crcext=0;
       //unsigned short value;
       char data[100];
      Log.Verbose("\nCMD =");Log.Verbose(String(inCommand));
      //int lastI=inCommand.lastIndexOf(',');
      //Serial.println(lastI);
//      String crcstr=inCommand.substring(lastI+1,inCommand.length()-1);
//      if(CrcEn)
//      {
//        Serial.println(crcstr);
//        crcext=crcstr.toInt();
//        inCommand.toCharArray(data,lastI);
//        Serial.println(data);
//        crc.clearCrc();
//        Serial.print("crc ext=");
//        Serial.println(crcext,HEX);
//        CrcCalc = crc.XModemCrc((byte *)data,0,lastI+1);
//        Serial.print("crc calc=");
//        Serial.println(CrcCalc,HEX);
//
//        if(crcext != CrcCalc) 
//        {
//          erFlag=true;
//          String s1="CRC error: calc=";
//          s1.concat(CrcCalc);
//          WriteErrMsg(s1);
//        //errMsg.concat(CrcCalc);
//        //Serial.println(errMsg);
//        }
//      }
      
      Nfield=(uint8_t)stringParser(inCommand);
      Cmd=(uint8_t)Field[Fcmd];
  
//  Serial.print("Cmd=");Serial.println(Cmd);
Log.Verbose("\nerrMsg="); Log.Verbose(String(errMsg));
Log.Verbose("\nNfield="); Log.Verbose(String(Nfield,DEC));Log.Verbose("\n");
      //inCommand="";
      //CmdCall();
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
  s.toCharArray(errMsg,MAX_ERR_MSG);
  return;
}


