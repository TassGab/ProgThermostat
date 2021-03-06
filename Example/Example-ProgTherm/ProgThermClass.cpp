#include "ProgThermClass.h"

StatusCs::StatusCs(void)  //Default Constructor
{
Status.Uartlev=_Verbose;
Status.ZBlev=_Info;
Status.Mode=_Daily;
Status.ClockPer=60;
Status.TimeAdj=0;
GetEEoffAddress=5;
return;
}

void StatusCs::NextState(Mode_en nextMode)
{
  Status.LastState=Status.Mode;
  Status.Mode=nextMode;  
  return;
}
void StatusCs::BackLastState()
{
  Mode_en tempstate=Status.LastState;
  Status.LastState=Status.Mode;
  Status.Mode=tempstate;
  return;
}

void StatusCs::SaveToEEProm()
{
EEPROM.put(GetEEoffAddress,Status);
Log.Verbose("EEPROM write at=");
Log.Verbose(String(GetEEoffAddress));Log.Verbose("\n");
Log.Verbose("Struct Size=");
Log.Verbose(String(sizeof(Status)));Log.Verbose("\n");
}
void StatusCs::ReadFromEEProm()
{
  EEPROM.get(GetEEoffAddress,Status);
 Log.LogLevel=Status.Uartlev;
 Log.ZBen=true;
 Log.ZBLogLevel=Status.ZBlev;
 Log.AutoCR=false;
}
void StatusCs::ChangePar(Log_en _Uartlev,Log_en _ZBlev,Mode_en _Mode,uint8_t _ClockPer, int _TimeAdj)
{
 Status.Uartlev=_Uartlev;
 Status.ZBlev=_ZBlev;
 Status.Mode=_Mode;
 Status.ClockPer=_ClockPer;
 Status.TimeAdj=_TimeAdj;
 SaveToEEProm();
 return;
}
