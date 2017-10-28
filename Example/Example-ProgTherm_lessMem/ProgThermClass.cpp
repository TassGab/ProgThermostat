#include "ProgThermClass.h"

StatusCs::StatusCs(void)  //Default Constructor
{
  Status.Uartlev = _Verbose;
  Status.ZBlev = _Info;
  Status.Mode = _Daily;
  Status.ClockPer = 60;
  Status.TimeAdj = 0;
  Status.HeaterStatus = 0;
  //Status.EvCalled = 0;
  Status.LastState = 0;
  GetEEoffAddress = 5;
  return;
}

void StatusCs::NextState(Mode_en nextMode)
{
  Status.LastState = Status.Mode;
  Status.Mode = nextMode;
  return;
}

void StatusCs::BackLastState()
{
  Mode_en tempstate = Status.LastState;
  Status.LastState = Status.Mode;
  Status.Mode = tempstate;
  return;
}

void StatusCs::SaveToEEProm()
{
  EEPROM.put(GetEEoffAddress, Status);
  Log.Verbose(F("EEPROM write at="));
  Log.Verbose(String(GetEEoffAddress)); Log.Verbose(F("\n"));
  Log.Verbose(F("Struct Size="));
  Log.Verbose(String(sizeof(Status))); Log.Verbose(F("\n"));
}
void StatusCs::ReadFromEEProm()
{
  EEPROM.get(GetEEoffAddress, Status);
  Log.LogLevel = Status.Uartlev;
  Log.ZBen = true;
  Log.ZBLogLevel = Status.ZBlev;
  Log.AutoCR = false;
  StatPrint();
}
void StatusCs::ChangePar(Log_en _Uartlev, Log_en _ZBlev, Mode_en _Mode, uint8_t _ClockPer, int _TimeAdj)
{
  Status.Uartlev = _Uartlev;
  Status.ZBlev = _ZBlev;
  Status.Mode = _Mode;
  Status.ClockPer = _ClockPer;
  Status.TimeAdj = _TimeAdj;
  SaveToEEProm();
  return;
}
void StatusCs::StatPrint()
{
  Log.Info(F("\nStatus=["));
  Log.Info(F("Heater=")); Log.Info(String(Status.HeaterStatus ? "ON":"OFF"));
  Log.Info(F(",Mode=")); Log.Info(ModeToStr(Status.Mode));
  Log.Info(F(",UART=")); Log.Info(Log.LevToStr(Status.Uartlev));
  Log.Info(F(",ZB=")); Log.Info(Log.LevToStr(Status.ZBlev));
  Log.Info(F(",ClockPer=")); Log.Info(String(Status.ClockPer));
  Log.Info(F(",TimeAdj=")); Log.Info(String(Status.TimeAdj));
  Log.Info(F("]\n"));
}
String StatusCs::ModeToStr(Mode_en mod)
{
  String s = "";
  if (mod == _Manual) s = "Manual";
  else if (mod == _Daily) s = "Daily";
  else if (mod == _Once) s = "Once";
  else if (mod == _Out) s = "Out";
  else s = "Not set";
  return s;
}

