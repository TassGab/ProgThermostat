#include "Heater_scheduler_class.h"
#include <Arduino.h>
//#define debug
HeaterSchedulerCs::HeaterSchedulerCs(void)
{
  //GetEEoffAddres=0;
  GetEEdaystructsize = sizeof(Sched.Daily);
  //daystructsize=23;
  GetEEeventstructsize = sizeof(Sched.EventFuture);
  return;
}

String HeaterSchedulerCs::SetEventDay(timeDayOfWeek_t dow, uint8_t Evnum, TimeQuarter_sc TimeQ, EventState_sc En, SwitchState_sc SwSt, SwitchNum_sc SwN)
{
  String s = "Dow: ";
  Sched.Daily.dow = dow;
  Sched.Daily.EvNum = Evnum;
  Sched.Daily.Event.EvTimeQ = TimeQ;
  Sched.Daily.Event.EventCtrl.isEnabled = En;
  Sched.Daily.Event.EventCtrl.swTurn = SwSt;
  Sched. Daily.Event.EventCtrl.nSwitch = SwN;
  s += dayStr(dow);
  s += " ; EventNum: ";
  s += Evnum;
  s += " ; ";
  s += EventToStrShort(Sched.Daily.Event);
  return s;
}
String HeaterSchedulerCs::SetEventOnce(uint8_t Evnum,  time_t TimeT, EventState_sc En, SwitchState_sc SwSt, SwitchNum_sc SwN)
{
  String s = "";
  Sched.EventFuture.TimeEv = TimeT;
  Sched.EventFuture.EventCtrl.isEnabled = En;
  Sched.EventFuture.EventCtrl.swTurn = SwSt;
  Sched.EventFuture.EventCtrl.nSwitch = SwN;
  s += "EventNum: ";
  s += Evnum;
  s += " ; ";
  s += EventOnceToStrShort(Sched.EventFuture);
  return s;
}

time_t HeaterSchedulerCs::QuartToTime(TimeQuarter_sc qt)
{
  time_t tt;
  if (qt == 0)
  {
    tt = 5; //5 seconds  after midnight
  }
  else
  {
#ifdef debug
    tt = qt * 5 + 5; //for test every 5 seconds
#else
    tt = qt * SECS_PER_MIN * 15 + 5;
#endif
  }
  Log.Verbose(F("\ntime="));Log.Verbose(String(tt));Log.Verbose(F("\n"));
  return tt;
}
String HeaterSchedulerCs::EventToStrShort(EventType_sc Ev)
{
  String s = "[";
  s += Ev.EvTimeQ;
  s += ",";
  s += Ev.EventCtrl.isEnabled;
  s += ",";
  s += Ev.EventCtrl.swTurn;
  s += ",";
  s += Ev.EventCtrl.nSwitch;
  s += "]";
  return s;
}
String HeaterSchedulerCs::EventOnceToStrShort(EventFuture_sc Ev)
{
  String s = "[";
  s += TimeToStr(Ev.TimeEv);
  s += ",";
  s += Ev.EventCtrl.isEnabled;
  s += ",";
  s += Ev.EventCtrl.swTurn;
  s += ",";
  s += Ev.EventCtrl.nSwitch;
  s += "]";
  return s;
}
String HeaterSchedulerCs::EventToStrLong(EventType_sc Ev)
{
  String s = "[Quarter: ";
  s += Ev.EvTimeQ;
  s += " (time: ";
  //Serial.print("\nEvent=[time: "));
  s += TimeToStr(QuartToTime(Ev.EvTimeQ));
  //Serial.print(F(" ; En: "));
  s += ") ; En:";
  //Serial.print(Ev.EventCtrl.isEnabled);
  s += Ev.EventCtrl.isEnabled ? "Enabled" : "Disabled";
  //Serial.print(F(" ; SwState: "));
  s += " ; SwState: ";
  //Serial.print(Ev.EventCtrl.swTurn);
  s += Ev.EventCtrl.swTurn ? "ON" : "OFF";
  //Serial.print(F(" ; SwNum; "));
  s += " ; SwNum; ";
  //Serial.print(Ev.EventCtrl.nSwitch);
  s += Ev.EventCtrl.nSwitch;
  //Serial.println("]");
  s += "]";
  return s;
}
String HeaterSchedulerCs::TimeToStr(time_t tt)
{
  String s = "";
  s += hour(tt);
  s += ":";
  s += minute(tt);
  s += ":";
  s += second(tt);
  //  Serial.print("\n"));
  //  Serial.print(hour(tt));
  //  Serial.print(":");
  //  Serial.print(minute(tt));
  //  Serial.print(":");
  //  Serial.println(second(tt));
  return s;
}
time_t HeaterSchedulerCs::SetTimeOfDay(uint8_t hh, uint8_t mm, uint8_t ss)
{
  tmElements_t tt;
  tt.Hour = hh;
  tt.Minute = mm;
  tt.Second = ss;
  //Log.Verbose("\ntime of day=");Log.Verbose(String(tt));Log.Verbose("\n"));
  return makeTime(tt);
}
time_t HeaterSchedulerCs::SetTimeEvent(uint8_t hh, uint8_t mm, uint8_t ss, uint8_t dd, uint8_t mo, uint8_t yy)
{
  tmElements_t tt;
  tt.Hour = hh;
  tt.Minute = mm;
  tt.Second = ss;
  tt.Day = dd;
  tt.Month = mo;
  tt.Year = CalendarYrToTm(yy);
  //Log.Verbose("\ntime=");Log.Verbose(String(tt));Log.Verbose("\n"));
  return makeTime(tt);
}
void HeaterSchedulerCs::RdEEPROMday(timeDayOfWeek_t dow, uint8_t _evnum)
{
  DaySchedule_sc daysc;
  int addr = int(GetEEdaystructsize * (nEventPerDay * (dow - 1) + _evnum)) + GetEEoffAddress;
  EEPROM.get(addr, daysc);
  Sched.Daily = DayDeepCp(daysc);
  Log.Debug(F("EEPROM read at="));
  Log.Debug(String(addr)); Log.Debug(F("\n"));
  //for (uint8_t i=0; i< nEventPerDay ;i++){
  //Log.Verbose(SetEventDay(dow,i,daysc.Event[i].EvTimeQ,daysc.Event[i].EventCtrl.isEnabled,daysc.Event[i].EventCtrl.nSwitch,daysc.Event[i].EventCtrl.swTurn));Log.Verbose("\n"));
  Log.Debug(F("EE Read: ")); Log.Debug(EventToStrShort(daysc.Event)); Log.Debug(F("\n"));
  Log.Verbose(F("Sched Read: ")); Log.Verbose(EventToStrShort(Sched.Daily.Event)); Log.Verbose(F("\n"));
}
void HeaterSchedulerCs::RdEEPROMevent(uint8_t _evnum)
{
  EventFuture_sc evsc;
  int addr = 0;
  addr = int(GetEEeventstructsize * (_evnum)) + GetEEdaystructsize * 7 * nEventPerDay + GetEEoffAddress;
  EEPROM.get(addr, evsc);
  Sched.EventFuture = EvDeepCp(evsc);
  Log.Debug(F("EEPROM read at="));
  Log.Debug(String(addr)); Log.Debug(F("\n"));
  return;
}
void HeaterSchedulerCs:: WrEEPROMdayEv(DaySchedule_sc daysc)
{
  int addr;
  addr = int(GetEEdaystructsize * (nEventPerDay * (daysc.dow - 1) + daysc.EvNum)) + GetEEoffAddress;
  for (int _add = addr; _add <= (addr + GetEEdaystructsize); _add++) {
    EEPROM.write(_add, 0);
  }//delete
  EEPROM.put(addr, daysc);
  Log.Debug(F("EEPROM write at="));
  Log.Debug(String(addr)); Log.Debug(F("\n"));
  return;
}
void HeaterSchedulerCs:: WrEEPROMevent(uint8_t _evnum, EventFuture_sc evsc)
{
  int addr;
  addr = int(GetEEeventstructsize * (_evnum)) + GetEEdaystructsize * 7 * nEventPerDay + GetEEoffAddress;
  for (int _add = addr; _add <= (addr + GetEEeventstructsize); _add++) {
    EEPROM.write(_add, 0);
  }
  EEPROM.put(addr, evsc);
  Log.Debug(F("EEPROM write at="));
  Log.Debug(String(addr)); Log.Debug(F("\n"));
}
DaySchedule_sc HeaterSchedulerCs::DayDeepCp(DaySchedule_sc orig)
{

  DaySchedule_sc dest;
  dest.dow = orig.dow;
  dest.EvNum = orig.EvNum;
  //   for (int i=0; i< nEventPerDay ;i++)
  //   {
  dest.Event.EvTimeQ = orig.Event.EvTimeQ;
  dest.Event.EventCtrl.isEnabled = orig.Event.EventCtrl.isEnabled;
  dest.Event.EventCtrl.nSwitch = orig.Event.EventCtrl.nSwitch;
  dest.Event.EventCtrl.swTurn = orig.Event.EventCtrl.swTurn;
  Log.Verbose(F("\nCopy Read: ")); Log.Verbose(EventToStrShort(dest.Event)); Log.Verbose(F("\n"));
  //   }
  return dest;
}
EventFuture_sc HeaterSchedulerCs::EvDeepCp(EventFuture_sc orig)
{
  EventFuture_sc dest;
  dest.TimeEv = orig.TimeEv;
  dest.EventCtrl.isEnabled = orig.EventCtrl.isEnabled;
  dest.EventCtrl.nSwitch = orig.EventCtrl.nSwitch;
  dest.EventCtrl.swTurn = orig.EventCtrl.swTurn;
  return dest;
}
void HeaterSchedulerCs::EEPromDefault()
{
  for (int _dow = 1; _dow <= 7; _dow++)
  {
    Sched.Daily.dow = _dow + 1;
#ifdef debug
    Log.Info(SetEventDay(_dow, 0, 0, evEN, swON, sw1)); Log.Info(F("\n"));
    WrEEPROMdayEv(Sched.Daily);
    Log.Info(SetEventDay(_dow, 1, 1, evEN, swOFF, sw1)); Log.Info(F("\n"));
    WrEEPROMdayEv(Sched.Daily);
    Log.Info(SetEventDay(_dow, 2, 2, evEN, swON, sw1)); Log.Info(F("\n"));
    WrEEPROMdayEv(Sched.Daily);
    Log.Info(SetEventDay(_dow, 3, 3, evEN, swOFF, sw1)); Log.Info(F("\n"));
    WrEEPROMdayEv(Sched.Daily);
    Log.Info(SetEventDay(_dow, 4, 4, evEN, swON, sw1)); Log.Info(F("\n"));
    WrEEPROMdayEv(Sched.Daily);
    Log.Info(SetEventDay(_dow, 5, 5, evEN, swOFF, sw1)); Log.Info(F("\n"));
    WrEEPROMdayEv(Sched.Daily);
#else
    Sched.Daily.EvNum=0;
    Sched.Daily.Event.EvTimeQ = 26; //6:30
    Sched.Daily.Event.EventCtrl.isEnabled = evEN;
    Sched.Daily.Event.EventCtrl.nSwitch = sw1;
    Sched.Daily.Event.EventCtrl.swTurn = swON;
    WrEEPROMdayEv(Sched.Daily);
    Sched.Daily.EvNum=1;
    Sched.Daily.Event.EvTimeQ = 32; //8:00
    Sched.Daily.Event.EventCtrl.isEnabled = evEN;
    Sched.Daily.Event.EventCtrl.nSwitch = sw1;
    Sched.Daily.Event.EventCtrl.swTurn = swOFF;
    WrEEPROMdayEv(Sched.Daily);
        Sched.Daily.EvNum=2;
    Sched.Daily.Event.EvTimeQ = 48; //12:00
    Sched.Daily.Event.EventCtrl.isEnabled = evEN;
    Sched.Daily.Event.EventCtrl.nSwitch = sw1;
    Sched.Daily.Event.EventCtrl.swTurn = swON;
    WrEEPROMdayEv(Sched.Daily);
        Sched.Daily.EvNum=3;
    Sched.Daily.Event.EvTimeQ = 56; //14:00
    Sched.Daily.Event.EventCtrl.isEnabled = evEN;
    Sched.Daily.Event.EventCtrl.nSwitch = sw1;
    Sched.Daily.Event.EventCtrl.swTurn = swOFF;
    WrEEPROMdayEv(Sched.Daily);
        Sched.Daily.EvNum=4;
    Sched.Daily.Event.EvTimeQ = 64; //16:00
    Sched.Daily.Event.EventCtrl.isEnabled = evEN;
    Sched.Daily.Event.EventCtrl.nSwitch = sw1;
    Sched.Daily.Event.EventCtrl.swTurn = swON;
    WrEEPROMdayEv(Sched.Daily);
        Sched.Daily.EvNum=5;
    Sched.Daily.Event.EvTimeQ = 100; //22:30
    Sched.Daily.Event.EventCtrl.isEnabled = evEN;
    Sched.Daily.Event.EventCtrl.nSwitch = sw1;
    Sched.Daily.Event.EventCtrl.swTurn = swOFF;
    WrEEPROMdayEv(Sched.Daily);
#endif
  }
#ifdef debug
  Log.Info(SetEventOnce(0, SetTimeEvent(0, 1, 0, 26, 10, 2017), evEN, swON, sw1)); Log.Info("\n"));
  //  Log.Info(SetEventOnce(1,SetTimeEvent(0,1,5,26,10,2017),evEN,swOFF,sw1));Log.Info("\n"));
#else
  Sched.EventFuture.TimeEv = SetTimeEvent(16, 0, 0, 5, 11, 17); //
  Sched.EventFuture.EventCtrl.isEnabled = evEN;
  Sched.EventFuture.EventCtrl.nSwitch = sw1;
  Sched.EventFuture.EventCtrl.swTurn = swON;

#endif
  WrEEPROMevent(0, Sched.EventFuture);

  Sched.EEPROMempty = 1;
  int addr;
  addr = int(GetEEeventstructsize * 2 + GetEEdaystructsize * 7 * nEventPerDay + GetEEoffAddress);
  EEPROM.write(addr, Sched.EEPROMempty);
  Log.Verbose(F("\nCheck EEPROM at ")); Log.Verbose(String(addr)); Log.Verbose(F("\n"));
  Log.Debug(F("\nEEPROM Default written"));
}
uint8_t HeaterSchedulerCs::GetEEPromEmpty()
{
  int addr;
  addr = int(GetEEeventstructsize * 2 + GetEEdaystructsize * 7 * nEventPerDay + GetEEoffAddress);
  Sched.EEPROMempty = EEPROM.read(addr);
  Log.Verbose(F("\nCheck EEPROM at ")); Log.Verbose(String(addr)); Log.Verbose(F(" Val=")); Log.Verbose(String(Sched.EEPROMempty)); Log.Verbose(F("\n"));
  return Sched.EEPROMempty;
}
void HeaterSchedulerCs::ClearEEProm()
{
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}
bool HeaterSchedulerCs::GetNextDayEv(timeDayOfWeek_t _dow)
{
  time_t _now = now();
  Log.Verbose(F("\nTime in sec="));Log.Verbose(String(elapsedSecsToday(_now)));Log.Verbose(F("\n"));
  for (int _ev = 0; _ev < nEventPerDay; _ev++)
  {
    RdEEPROMday(_dow, _ev);
    if ((elapsedSecsToday(_now) <= Sched.Daily.Event.EvTimeQ*15*SECS_PER_MIN) && (Sched.Daily.Event.EventCtrl.isEnabled))
    {
      Log.Debug(F("\nEvent found in today =")); Log.Debug(String(_ev)); Log.Debug(F("\n"));
      return false;
    }
  }
  Log.Debug(F("\nEvent Not found in today --> Next event is midnight\n"));
  return true;
}
