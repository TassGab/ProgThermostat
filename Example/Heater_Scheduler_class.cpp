#include "Heater_scheduler_class.h"
#include <Arduino.h>
HeaterSchedulerCs::HeaterSchedulerCs(void)
{
  //GetEEoffAddres=0;
  GetEEdaystructsize=sizeof(Sched.WeekSched[0]);
  //daystructsize=23;  
  GetEEeventstructsize=sizeof(Sched.EventFuture[0]);
  return;
}
//int HeaterSchedulerCs::GetEEoffAddress()
// {
//  return 0; 
// }
// int HeaterSchedulerCs::GetEEdaystructsize()
// {
//  return sizeof(Sched. WeekSched[0]);
// }
// int HeaterSchedulerCs::GetEEeventstructsize()
// {
//  return sizeof(Sched.EventFuture[0]);
//  }
String HeaterSchedulerCs::SetEventDay(timeDayOfWeek_t dow, EventNum_sc Evnum, TimeQuarter_sc TimeQ, EventState_sc En, SwitchState_sc SwSt, SwitchNum_sc SwN)
 {
   String s="Dow: ";   
   Sched.WeekSched[dow-1].dow=dow;
   Sched.WeekSched[dow-1].Event[Evnum].EvTimeQ=TimeQ;
   Sched.WeekSched[dow-1].Event[Evnum].EventCtrl.isEnabled=En;
   Sched.WeekSched[dow-1].Event[Evnum].EventCtrl.swTurn=SwSt;
   Sched. WeekSched[dow-1].Event[Evnum].EventCtrl.nSwitch=SwN;
   s+=dayStr(dow);
   s+=" ; EventNum: ";
   s+=Evnum;
   s+=" ; ";
   s+=EventToStrShort(Sched.WeekSched[dow-1].Event[Evnum]);
   return s;
 }
 String HeaterSchedulerCs::SetEventOnce(EventNum_sc Evnum,  time_t TimeT, EventState_sc En, SwitchState_sc SwSt, SwitchNum_sc SwN)
 {
   String s="";   
   Sched.EventFuture[Evnum].TimeEv=TimeT;
   Sched.EventFuture[Evnum].EventCtrl.isEnabled=En;
   Sched.EventFuture[Evnum].EventCtrl.swTurn=SwSt;
   Sched.EventFuture[Evnum].EventCtrl.nSwitch=SwN;
   s+="EventNum: ";
   s+=Evnum;
   s+=" ; ";
   s+=EventOnceToStrShort(Sched.EventFuture[Evnum]);
   return s;
 }

time_t HeaterSchedulerCs::QuartToTime(TimeQuarter_sc qt)
{
  time_t tt;
  if (qt==0)
  {
    tt=5; //one second  after midnight
  }
  else
  {
    //tt=qt*SECS_PER_MIN * 15;
    tt=qt*5+5; //for test every 5 seconds
  }
  return tt;
}
String HeaterSchedulerCs::EventToStrShort(EventType_sc Ev)
{
  String s="[";
  s+=Ev.EvTimeQ;
  s+=",";
  s+=Ev.EventCtrl.isEnabled;
  s+=",";
  s+=Ev.EventCtrl.swTurn;
  s+=",";
  s+=Ev.EventCtrl.nSwitch;
  s+="]";
  return s;
}
String HeaterSchedulerCs::EventOnceToStrShort(EventFuture_sc Ev)
{
  String s="[";
  s+=TimeToStr(Ev.TimeEv);
  s+=",";
  s+=Ev.EventCtrl.isEnabled;
  s+=",";
  s+=Ev.EventCtrl.swTurn;
  s+=",";
  s+=Ev.EventCtrl.nSwitch;
  s+="]";
  return s;
}
String HeaterSchedulerCs::EventToStrLong(EventType_sc Ev)
 {
  String s="[Quarter: ";
  s+=Ev.EvTimeQ;
  s+=" (time: ";
  //Serial.print("\nEvent=[time: ");
  s+=TimeToStr(QuartToTime(Ev.EvTimeQ));
  //Serial.print(" ; En: ");
  s+=") ; En:"; 
  //Serial.print(Ev.EventCtrl.isEnabled);
  s+=Ev.EventCtrl.isEnabled ? "Enabled":"Disabled";
  //Serial.print(" ; SwState: ");
  s+=" ; SwState: ";
  //Serial.print(Ev.EventCtrl.swTurn);
  s+=Ev.EventCtrl.swTurn ? "ON":"OFF";
  //Serial.print(" ; SwNum; ");
  s+=" ; SwNum; ";
  //Serial.print(Ev.EventCtrl.nSwitch);
  s+=Ev.EventCtrl.nSwitch;
  //Serial.println("]");
  s+="]";
  return s;
 }
String HeaterSchedulerCs::TimeToStr(time_t tt)
 {
  String s="";
  s+=hour(tt);
  s+=":";
  s+=minute(tt);
  s+=":";
  s+=second(tt);
//  Serial.print("\n");
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
  tt.Hour=hh;
  tt.Minute=mm;
  tt.Second=ss;
  return makeTime(tt);
 }
 time_t HeaterSchedulerCs::SetTimeEvent(uint8_t hh, uint8_t mm, uint8_t ss, uint8_t dd, uint8_t mo, uint8_t yy)
 {
  tmElements_t tt;
  tt.Hour=hh;
  tt.Minute=mm;
  tt.Second=ss;
  tt.Day=dd;
  tt.Month=mo;
  tt.Year=CalendarYrToTm(yy);
  return makeTime(tt);
 }
 void HeaterSchedulerCs::RdEEPROMday(timeDayOfWeek_t dow)
 {
   DaySchedule_sc daysc;
   int addr=int(GetEEdaystructsize*(dow-1))+GetEEoffAddress;
   EEPROM.get(addr,daysc);
   DayDeepCp(daysc,Sched.WeekSched[dow-1]);
   Log.Verbose("EEPROM read at=");
   Log.Verbose(String(addr));Log.Verbose("\n");
 }
void HeaterSchedulerCs::RdEEPROMevent(uint8_t _evnum)
 {
  EventFuture_sc evsc;
   int addr=0;
   addr=int(GetEEeventstructsize*(_evnum))+GetEEdaystructsize*7+GetEEoffAddress;
   EEPROM.get(addr,evsc);
   EvDeepCp(evsc,Sched.EventFuture[_evnum]);
   Log.Verbose("EEPROM read at=");
   Log.Verbose(String(addr));Log.Verbose("\n");
   return;
 }
 void HeaterSchedulerCs:: WrEEPROMday(DaySchedule_sc daysc)
 {
  int addr;  
  addr=int(GetEEdaystructsize*(daysc.dow-1))+GetEEoffAddress;
  EEPROM.put(addr,daysc);
  Log.Verbose("EEPROM write at=");
  Log.Verbose(String(addr));Log.Verbose("\n");
  return;
 }
  void HeaterSchedulerCs:: WrEEPROMevent(uint8_t _evnum,EventFuture_sc evsc)
 {
  int addr;  
  addr=int(GetEEeventstructsize*(_evnum))+GetEEdaystructsize*7+GetEEoffAddress;
  EEPROM.put(addr,evsc);
  Log.Verbose("EEPROM write at=");
  Log.Verbose(String(addr));Log.Verbose("\n");
 }
 void HeaterSchedulerCs::DayDeepCp(DaySchedule_sc orig, DaySchedule_sc dest)
 {
   dest.dow=orig.dow;
   for (int i=0; i< nEventPerDay ;i++)
   {
    dest.Event[i].EvTimeQ=orig.Event[i].EvTimeQ;
    dest.Event[i].EventCtrl.isEnabled=orig.Event[i].EventCtrl.isEnabled;
    dest.Event[i].EventCtrl.nSwitch=orig.Event[i].EventCtrl.nSwitch;
    dest.Event[i].EventCtrl.swTurn=orig.Event[i].EventCtrl.swTurn;
    return;
   }
 }
 void HeaterSchedulerCs::EvDeepCp(EventFuture_sc orig,EventFuture_sc dest)
 {
   dest.TimeEv=orig.TimeEv;
   dest.EventCtrl.isEnabled=orig.EventCtrl.isEnabled;
   dest.EventCtrl.nSwitch=orig.EventCtrl.nSwitch;
   dest.EventCtrl.swTurn=orig.EventCtrl.swTurn;
   return;
 }
 void HeaterSchedulerCs::EEPromDefault()
 {
  for (int _dow=0; _dow<7; _dow++)
  {
    Sched.WeekSched[_dow].dow=_dow+1;
    WrEEPROMday(Sched.WeekSched[_dow]);
  }
  WrEEPROMevent(0,Sched.EventFuture[0]);
  WrEEPROMevent(1,Sched.EventFuture[1]);
 }
 

