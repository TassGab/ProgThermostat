#include "Logging_class.h"
//#define debug

LoggingCs::LoggingCs()
{
  LogLevel=ZBLogLevel=_Verbose;
  AutoCR=false;
  ZBen=false;
}
void LoggingCs::Verbose(String s)
{
  if(_Verbose>=Log.LogLevel)
  {
    Print(s);
  }
  if((_Verbose>=Log.ZBLogLevel)&(Log.ZBen))
  {
  SendZB(s);
  #ifdef debug
    Serial.print(F("ZB:"));
    Serial.print(s);
    if(Log.AutoCR) Serial.println();
  #endif
  }
}
void LoggingCs::Debug(String s)
{
  if(_Debug>=Log.LogLevel)
  {
    Print(s);
  }
  if((_Debug>=Log.ZBLogLevel)&(Log.ZBen))
  {
  SendZB(s);
  #ifdef debug
    Serial.print(F("ZB:"));
    Serial.print(s);
    if(Log.AutoCR) Serial.println();
  #endif
  }
}
void LoggingCs::Info(String s)
{
  if(_Info>=Log.LogLevel)
  {
    Print(s);
  }
  if((_Info>=Log.ZBLogLevel)&(Log.ZBen))
  {
   SendZB(s);
 #ifdef debug
    Serial.print(F("ZB:"));
    Serial.print(s);
    if(Log.AutoCR) Serial.println();
  #endif
  }
}
void LoggingCs::Error(String s)
{
  if(_Error>=Log.LogLevel)
  {
    Print(s);
  }
  if((_Error>=Log.ZBLogLevel)&(Log.ZBen))
  {
    SendZB(s);
	#ifdef debug
    Serial.print(F("ZB:"));
    Serial.print(s);
    if(Log.AutoCR) Serial.println();
  #endif
  }
}
void LoggingCs::Print(String s)
{
  Serial.print(s);
  if(Log.AutoCR) Serial.println();
  
}
//void LoggingCs::Print(char s[])
//{
//  Serial.print(s);
//  if(Log.AutoCR) Serial.println();  
//}

String LoggingCs::LevToStr(Log_en lev)
{
  String s="";
  if(lev==_Verbose) s="Verbose";
  else if(lev==_Debug) s="Debug";
  else if(lev==_Info) s="Info";
  else if(lev==_Error) s="Error";
  else s="Not Set";
  return s;
}
void LoggingCs::SendZB(String s)
{
//  #ifdef zigbee
//  byte data[CHB_MAX_PAYLOAD];
//  unsigned int len;
//  if(Log.AutoCR) s+='\n';
// 
//  len=byte(s.length());
//  Log.Verbose(F("(len="));Log.Verbose(String(len));Log.Verbose(F(")\n"));
//  //s.toCharArray(data,len);
//  #ifdef crc_calc
//    byte value = 0;
//    value=CRC8((char *)data,len);
//    #ifdef debug
//    Log.Verbose(F("\nCrc:"));Log.Verbose(String(value));Log.Verbose(F("\n"));
//    #endif
//    s+=String(value);
//  #endif
//  s+='\0';
//  len=byte(s.length());
//  s.toCharArray(data,len);
//  chibiTx(BROADCAST_ADDR, (byte *)data,len);
//  #endif
  #ifdef zigbee
  byte data[CHB_MAX_PAYLOAD];
  unsigned int len;  
  msgTx+=s;
  len=byte(msgTx.length());  
  char endCh=msgTx[len-1];
  if(endCh=='\n')
  {
  if(!Log.AutoCR) 
  {
    msgTx.remove(len-1,1);
    len--;
  }
  //Log.Verbose(F("(len="));Log.Verbose(String(len));Log.Verbose(F(")\n"));
  //s.toCharArray(data,len);
  #ifdef crc_calc
    byte value = 0;
    value=CRC8((char *)data,len);
    #ifdef debug
    Log.Verbose(F("\nCrc:"));Log.Verbose(String(value));Log.Verbose(F("\n"));
    #endif
    msgTx+=String(value);
  #endif
  msgTx+='\0';
  len++;
  msgTx.toCharArray(data,len);
  msgTx="";
  chibiTx(BROADCAST_ADDR, (byte *)data,len);
  }
  #endif
  return;
}

//CRC-8 - based on the CRC8 formulas by Dallas/Maxim
//code released under the therms of the GNU GPL 3.0 license
byte LoggingCs::CRC8(const byte *data, byte len) {
  byte crc = 0x00;
  while (len--) {
    byte extract = *data++;
    for (byte tempI = 8; tempI; tempI--) {
      byte sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum) {
        crc ^= 0x8C;
      }
      extract >>= 1;
    }
  }
  return crc;
}
LoggingCs Log=LoggingCs(); //instance for the user

