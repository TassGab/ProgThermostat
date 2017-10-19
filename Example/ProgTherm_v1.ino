/*
 * 10/09/2017
 * Gabriele Tasselli
 * 
 * Scheduler for Chorno Thermostat
 * weekly scheduler to be used with TimeAlarm library
 * The schedule is stored in the eeprom
 * For memory optimization the resolution is 15 minutes.
 * Intially 4 events per day are allocated
 */
 #include "CmdParserClass.h"
#include "Heater_scheduler_class.h"
#include <TimeAlarms.h>

//#include <Logging_class.h>
//void UpdateSched();
//void digitalClockDisplay();
//void ExCommand(uint8_t);
AlarmId id0,id1,id2,id3,id4,id5,idse0,idse1;
EventNum_sc ev; //Global var for event of the day 
HeaterSchedulerCs HS=HeaterSchedulerCs(); //class instance
CmdParserClass CP=CmdParserClass();
String usbCommand="";
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
Log.LogLevel=_Verbose;
Log.ZBen=true;
Log.ZBLogLevel=_Error;
Log.AutoCR=false;
//  Serial.print("Day Size=");
//  Serial.println(sizeof(HS.Sched.WeekSched[0]),DEC);
  HS.EEPromDefault();
  Log.Info(HS.SetEventDay(dowTuesday,Event0,0,evEN,swON,sw1));Log.Info("\n");
  Log.Info(HS.SetEventDay(dowTuesday,Event1,1,evEN,swOFF,sw1));Log.Info("\n");
  Log.Info(HS.SetEventDay(dowTuesday,Event2,2,evEN,swON,sw1));Log.Info("\n");
  Log.Info(HS.SetEventDay(dowTuesday,Event3,3,evEN,swOFF,sw1));Log.Info("\n");
  Log.Info(HS.SetEventDay(dowTuesday,Event4,4,evEN,swON,sw1));Log.Info("\n");
  Log.Info(HS.SetEventDay(dowTuesday,Event5,5,evEN,swOFF,sw1));Log.Info("\n");
  Log.Info(HS.SetEventOnce(0,HS.SetTimeEvent(0,1,0,26,10,2017),evEN,swON,sw1));Log.Info("\n");
  Log.Info(HS.SetEventOnce(1,HS.SetTimeEvent(0,1,5,26,10,2017),evEN,swOFF,sw1));Log.Info("\n");
  setTime(23,59,50,16,10,17); // set time
   //time_t now_t=now();
//   time_t now_t;
//   now_t=HS.SetTimeOfDay(3,10,10);
//   Serial.println(HS.TimeToStr(now_t));
//   Serial.print("now in quarter=");
//   Serial.println(TimeToQuart(now_t));
   Alarm.alarmRepeat(0,0,0, UpdateSched);  // update schedule at midnight
   
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalClockDisplay();

  if (Serial.available() > 0) 
   {
      //Log.Info("\nReceived a command\n");
      Serial.println("\nReceived a command\n");
      usbCommand+=Serial.readString();
      CP.Parse(usbCommand);
      usbCommand="";
    if(CP.erFlag)
    {
      Log.Error(CP.errMsg);
    }
    else
    {
      
      ExCommand(CP.Cmd);
    }
   }
   Alarm.delay(1000); // wait one second between clock display

}//end loop
/*
 * Commands
 */
void ExCommand(uint8_t cmd)
{
  switch(cmd)
  {
    case 1: //set time
      //format: #1,hh,mm,ss,dd,mo,yy.
      if (CP.Nfield==7)
      {
        Log.Info("\nCommand set time: ");
        uint8_t hh=CP.Field[1]; Log.Verbose(String(hh));Log.Verbose(":");
        uint8_t mm=CP.Field[2]; Log.Verbose(String(mm));Log.Verbose(":");
        uint8_t ss=CP.Field[3]; Log.Verbose(String(ss));Log.Verbose("\t");
        uint8_t dd=CP.Field[4]; Log.Verbose(String(dd));Log.Verbose("/");
        uint8_t mo=CP.Field[5]; Log.Verbose(String(mo));Log.Verbose("/");
        uint8_t yy=CP.Field[6]; Log.Verbose(String(yy));Log.Verbose("\n");
        setTime(hh,mm,ss,dd,mo,yy);
        //Log.Info(HS.TimeToStr(now()));
        //Log.Info("\n");
      }
    break;
    case 2: //set alarm eventDay
    //format: #2,dow,evnum,quarter,en,on/off,sw1.
    if (CP.Nfield==7)
    {
      Log.Info("\nCommand set DayEvent: ");
      uint8_t _dow=CP.Field[1]; Log.Verbose(String(_dow));Log.Verbose(",");
      uint8_t _evnum=CP.Field[2]; Log.Verbose(String(_evnum));Log.Verbose(",");
      uint8_t _Tqua=CP.Field[3]; Log.Verbose(String(_Tqua));Log.Verbose(",");
      uint8_t _en=CP.Field[4]; Log.Verbose(String(_en));Log.Verbose(",");
      uint8_t _onoff=CP.Field[5]; Log.Verbose(String(_onoff));Log.Verbose(",");
      uint8_t _swn=CP.Field[6]; Log.Verbose(String(_swn));Log.Verbose("\n");
      Log.Info(HS.SetEventDay(_dow,_evnum,_Tqua,_en,_onoff,_swn));Log.Info("\n");
      //HS.WrEEPROMday(HS.Sched.WeekSched[_dow-1]);
    }
    break;
    case 3: //Save eventDay
    //format: #3,dow.
    if (CP.Nfield==2)
    {
      Log.Info("\nCommand READ DayEvent: ");
      uint8_t _dow=CP.Field[1]; Log.Verbose(String(_dow));Log.Verbose("\n");      
      HS.WrEEPROMday(HS.Sched.WeekSched[_dow-1]);
    }
    break;
    case 4: //Read alarm eventDay
    //format: #4,dow,evnum.
    if (CP.Nfield==3)
    {
      Log.Info("\nCommand READ DayEvent: ");
      uint8_t _dow=CP.Field[1]; Log.Verbose(String(_dow));Log.Verbose(",");
      uint8_t _evnum=CP.Field[2]; Log.Verbose(String(_evnum));Log.Verbose("\n");
      HS.RdEEPROMday(_dow);
      Log.Info(HS.EventToStrShort(HS.Sched.WeekSched[_dow-1].Event[_evnum]));Log.Info("\n");      
    }
    break;
    default:
     Log.Error("\nCommand Unknown\n");
  }
  return;
}
/*
 * Functions
 */
 void Event0R() {
  //AlarmId id=  Alarm.getTriggeredAlarmId();
  ev=0;
  Log.Info("Triggered Alarm=");Log.Info(String(ev)); Log.Info("\n");
  //Serial.println(ev,DEC);
  int dow=weekday()-1;
  // use Alarm.free() to disable a timer and recycle its memory.

  if (HS.Sched.WeekSched[dow].Event[ev].EventCtrl.swTurn==swON)
  {
    Log.Info("Switch ON\n");
  }
  else
  {
    Log.Info("Switch OFF\n");
  }
  Alarm.free(id0);
  // optional, but safest to "forget" the ID after memory recycled
  id0 = dtINVALID_ALARM_ID;
  // you can also use Alarm.disable() to turn the timer off, but keep
  // it in memory, to turn back on later with Alarm.enable().
  return;
}
void Event1R() {
  //AlarmId id=  Alarm.getTriggeredAlarmId();
  ev=1;
  Log.Info("Triggered Alarm=");Log.Info(String(ev)); Log.Info("\n");
  //Serial.println(ev,DEC);
  int dow=weekday()-1;
  // use Alarm.free() to disable a timer and recycle its memory.

  if (HS.Sched.WeekSched[dow].Event[ev].EventCtrl.swTurn==swON)
  {
    Log.Info("Switch ON\n");
  }
  else
  {
    Log.Info("Switch OFF\n");
  }
  Alarm.free(id1);
  // optional, but safest to "forget" the ID after memory recycled
  id1 = dtINVALID_ALARM_ID;
  // you can also use Alarm.disable() to turn the timer off, but keep
  // it in memory, to turn back on later with Alarm.enable().
    return;
}
void Event2R() {
  //AlarmId id=  Alarm.getTriggeredAlarmId();
  ev=2;
  Log.Info("Triggered Alarm=");Log.Info(String(ev)); Log.Info("\n");
  //Serial.println(ev,DEC);
  int dow=weekday()-1;
  // use Alarm.free() to disable a timer and recycle its memory.

  if (HS.Sched.WeekSched[dow].Event[ev].EventCtrl.swTurn==swON)
  {
    Log.Info("Switch ON\n");
  }
  else
  {
    Log.Info("Switch OFF\n");
  }
  Alarm.free(id2);
  // optional, but safest to "forget" the ID after memory recycled
  id2 = dtINVALID_ALARM_ID;
  // you can also use Alarm.disable() to turn the timer off, but keep
  // it in memory, to turn back on later with Alarm.enable().
    return;
}
void Event3R() {
  //AlarmId id=  Alarm.getTriggeredAlarmId();
  ev=3;
  Log.Info("Triggered Alarm=");Log.Info(String(ev)); Log.Info("\n");
  //Serial.println(ev,DEC);
  int dow=weekday()-1;
  // use Alarm.free() to disable a timer and recycle its memory.

  if (HS.Sched.WeekSched[dow].Event[ev].EventCtrl.swTurn==swON)
  {
    Log.Info("Switch ON\n");
  }
  else
  {
    Log.Info("Switch OFF\n");
  }
  Alarm.free(id3);
  // optional, but safest to "forget" the ID after memory recycled
  id3 = dtINVALID_ALARM_ID;
  // you can also use Alarm.disable() to turn the timer off, but keep
  // it in memory, to turn back on later with Alarm.enable().
    return;
}
void Event4R() {
  //AlarmId id=  Alarm.getTriggeredAlarmId();
  ev=4;
  Log.Info("Triggered Alarm=");Log.Info(String(ev)); Log.Info("\n");
  //Serial.println(ev,DEC);
  int dow=weekday()-1;
  // use Alarm.free() to disable a timer and recycle its memory.

  if (HS.Sched.WeekSched[dow].Event[ev].EventCtrl.swTurn==swON)
  {
    Log.Info("Switch ON\n");
  }
  else
  {
    Log.Info("Switch OFF\n");
  }
  Alarm.free(id4);
  // optional, but safest to "forget" the ID after memory recycled
  id4 = dtINVALID_ALARM_ID;
  // you can also use Alarm.disable() to turn the timer off, but keep
  // it in memory, to turn back on later with Alarm.enable().
    return;
}
void Event5R() {
  //AlarmId id=  Alarm.getTriggeredAlarmId();
  ev=5;
  Log.Info("Triggered Alarm=");Log.Info(String(ev)); Log.Info("\n");
  //Serial.println(ev,DEC);
  int dow=weekday()-1;
  // use Alarm.free() to disable a timer and recycle its memory.

  if (HS.Sched.WeekSched[dow].Event[ev].EventCtrl.swTurn==swON)
  {
    Log.Info("Switch ON\n");
  }
  else
  {
    Log.Info("Switch OFF\n");
  }
  Alarm.free(id5);
  // optional, but safest to "forget" the ID after memory recycled
  id5 = dtINVALID_ALARM_ID;
  // you can also use Alarm.disable() to turn the timer off, but keep
  // it in memory, to turn back on later with Alarm.enable().
    return;
}
void EventOnce0() {
  //AlarmId id=  Alarm.getTriggeredAlarmId();
  ev=0;
  Log.Info("Triggered Alarm Single Event=");Log.Info(String(ev)); Log.Info("\n");
  //Serial.println(ev,DEC);
  
  // use Alarm.free() to disable a timer and recycle its memory.

  if (HS.Sched.EventFuture[ev].EventCtrl.swTurn==swON)
  {
    Log.Info("Switch ON\n");
  }
  else
  {
    Log.Info("Switch OFF\n");
  }
  Alarm.free(idse0);
  // optional, but safest to "forget" the ID after memory recycled
  idse0 = dtINVALID_ALARM_ID;
  // you can also use Alarm.disable() to turn the timer off, but keep
  // it in memory, to turn back on later with Alarm.enable().
    return;
}
void EventOnce1() {
  //AlarmId id=  Alarm.getTriggeredAlarmId();
  ev=1;
  Log.Info("Triggered Alarm Single Event=");Log.Info(String(ev)); Log.Info("\n");
  //Serial.println(ev,DEC);
  
  // use Alarm.free() to disable a timer and recycle its memory.

  if (HS.Sched.EventFuture[ev].EventCtrl.swTurn==swON)
  {
    Log.Info("Switch ON\n");
  }
  else
  {
    Log.Info("Switch OFF\n");
  }
  Alarm.free(idse1);
  // optional, but safest to "forget" the ID after memory recycled
  idse1 = dtINVALID_ALARM_ID;
  // you can also use Alarm.disable() to turn the timer off, but keep
  // it in memory, to turn back on later with Alarm.enable().
    return;
}
void UpdateSched()
{
  Log.Info("update Schedule\n");
  int dow=weekday()-1;
//Serial.print(dow); 
  HS.RdEEPROMday(dow);
    if (HS.Sched.WeekSched[dow].Event[Event0].EventCtrl.isEnabled==evEN)
    {
      id0=Alarm.timerOnce(HS.QuartToTime(HS.Sched.WeekSched[dow].Event[Event0].EvTimeQ), Event0R);
      Log.Info("id0: ");
      Log.Info(HS.EventToStrShort(HS.Sched.WeekSched[dow].Event[Event0]));
      Log.Info("\n");
    }
     if (HS.Sched.WeekSched[dow].Event[Event1].EventCtrl.isEnabled==evEN)
    {
      id1=Alarm.timerOnce(HS.QuartToTime(HS.Sched.WeekSched[dow].Event[Event1].EvTimeQ), Event1R);
      Log.Info("id1: ");
      Log.Info(HS.EventToStrShort(HS.Sched.WeekSched[dow].Event[Event1]));
      Log.Info("\n");
    }
   if (HS.Sched.WeekSched[dow].Event[Event2].EventCtrl.isEnabled==evEN)
    {
      id2=Alarm.timerOnce(HS.QuartToTime(HS.Sched.WeekSched[dow].Event[Event2].EvTimeQ), Event2R);
      Log.Info("id2: ");
      Log.Info(HS.EventToStrShort(HS.Sched.WeekSched[dow].Event[Event2]));
      Log.Info("\n");
    }
    if (HS.Sched.WeekSched[dow].Event[Event3].EventCtrl.isEnabled==evEN)
    {
      id3=Alarm.timerOnce(HS.QuartToTime(HS.Sched.WeekSched[dow].Event[Event3].EvTimeQ), Event3R);
      Log.Info("id3: ");
      Log.Info(HS.EventToStrShort(HS.Sched.WeekSched[dow].Event[Event3]));
      Log.Info("\n");
    }
     if (HS.Sched.WeekSched[dow].Event[Event4].EventCtrl.isEnabled==evEN)
    {
      id4=Alarm.timerOnce(HS.QuartToTime(HS.Sched.WeekSched[dow].Event[Event4].EvTimeQ), Event4R);
      Log.Info("id4: ");
      Log.Info(HS.EventToStrShort(HS.Sched.WeekSched[dow].Event[Event4]));
      Log.Info("\n");
    }
     if (HS.Sched.WeekSched[dow].Event[Event5].EventCtrl.isEnabled==evEN)
    {
      id5=Alarm.timerOnce(HS.QuartToTime(HS.Sched.WeekSched[dow].Event[Event5].EvTimeQ), Event5R);
      Log.Info("id5: ");
      Log.Info(HS.EventToStrShort(HS.Sched.WeekSched[dow].Event[Event5]));
      Log.Info("\n");
    }
    HS.RdEEPROMevent(0);
    if (HS.Sched.EventFuture[0].EventCtrl.isEnabled==evEN)
    {
      idse0=Alarm.triggerOnce(HS.Sched.EventFuture[0].TimeEv, EventOnce0);
      Log.Info("idse0: ");
      Log.Info(HS.EventOnceToStrShort(HS.Sched.EventFuture[0]));
      Log.Info("\n");
    }
    HS.RdEEPROMevent(1);
    if (HS.Sched.EventFuture[1].EventCtrl.isEnabled==evEN)
    {
      idse1=Alarm.triggerOnce(HS.Sched.EventFuture[1].TimeEv, EventOnce1);
      Log.Info("idse1: ");
      Log.Info(HS.EventOnceToStrShort(HS.Sched.EventFuture[1]));
      Log.Info("\n");
    }
      return;
}
void digitalClockDisplay() {
  // digital clock display of the time
  Log.Info(String(hour()));
  printDigits(minute());
  printDigits(second());
  Log.Info("\n");
    return;
}

void printDigits(int digits) {
  Log.Info(":");
  if (digits < 10)
    Log.Info("0");
  Log.Info(String(digits));
    return;
}

