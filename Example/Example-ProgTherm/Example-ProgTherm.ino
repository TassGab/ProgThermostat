/*
 * 10/09/2017
 * Gabriele Tasselli
 * 
 * Scheduler for Programmable Thermostat
 * weekly scheduler to be used with TimeAlarm library
 * The schedule is stored in the eeprom
 * For memory optimization the resolution is 15 minutes.
 * Intially 4 events per day are allocated
 */
#include "CmdParserClass.h"
#include "Heater_scheduler_class.h"
#include "ProgThermClass.h"
#include <TimeAlarms.h>

AlarmId id0,id1,id2,id3,id4,id5,idse0,idse1;
//EventNum_sc ev; //Global var for event of the day 
StatusCs HStat=StatusCs(); //Declare the status class
HeaterSchedulerCs HS=HeaterSchedulerCs(); //class instance
CmdParserClass CP=CmdParserClass();
int loop_c=1;
String usbCommand="";
int SwHeaters=5;
//
void setup() {
  // put your setup code here, to run once:
Serial.begin(38400);

pinMode(SwHeaters,OUTPUT);
digitalWrite(SwHeaters,LOW);
//HS.ClearEEProm();
if(HS.GetEEPromEmpty()==0) 
  {
    Log.Debug("\nEEPROM write default...\n");
    HS.EEPromDefault();
    HStat.SaveToEEProm();
  }
HStat.ReadFromEEProm();
chibiInit();
Log.Verbose("\nStatus:");Log.Verbose("\n");
Log.Verbose("size=");Log.Verbose(String(sizeof(HStat.Status),DEC));Log.Verbose("\n");
Log.Verbose("mode=");Log.Verbose(String(HStat.Status.Mode));Log.Verbose("\n");
Log.Verbose("Loglev=");Log.Verbose(String(HStat.Status.Uartlev));Log.Verbose("\n");
Log.Verbose("ClockPeriod=");Log.Verbose(String(HStat.Status.ClockPer));Log.Verbose("\n");
setTime(23,59,50,16,10,17); // set time
Alarm.alarmRepeat(0,0,0, UpdateSched);  // update schedule at midnight   
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) 
   {
      Log.Info("\nReceived a command\n");
      //Serial.println("\nReceived a command\n");
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
  if (chibiDataRcvd() == true)
  { 
    int len;
    byte buf[CHB_MAX_PAYLOAD];  // this is where we store the received data    
    // retrieve the data and the signal strength
    len = chibiGetData(buf);
    // discard the data if the length is 0. that means its a duplicate packet
    if (len > 0)
    {
         Log.Info("\nReceived a command\n");
         CP.Parse(String((char *)buf));
    }
  }
   Alarm.delay(1000); // wait one second between clock display
  if(loop_c++==HStat.Status.ClockPer)
   {
    digitalClockDisplay();
    loop_c=1;
   }
}//end loop
/***************************************
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
        
        Log.Debug("\nCommand set time: ");
        uint8_t hh=CP.Field[1]; Log.Verbose(String(hh));Log.Verbose(":");
        uint8_t mm=CP.Field[2]; Log.Verbose(String(mm));Log.Verbose(":");
        uint8_t ss=CP.Field[3]; Log.Verbose(String(ss));Log.Verbose("\t");
        uint8_t dd=CP.Field[4]; Log.Verbose(String(dd));Log.Verbose("/");
        uint8_t mo=CP.Field[5]; Log.Verbose(String(mo));Log.Verbose("/");
        uint8_t yy=CP.Field[6]; Log.Verbose(String(yy));Log.Verbose("\n");
        setTime(hh,mm,ss,dd,mo,yy);
        Log.Info("\n#1:OK\n");
        //Log.Info(HS.TimeToStr(now()));
        //Log.Info("\n");
      }
    break;
    case 2: //set alarm eventDay
    //format: #2,dow,evnum,quarter,en,on/off,sw1.
    if (CP.Nfield==7)
    {
      Log.Debug("\nCommand set DayEvent: ");
      uint8_t _dow=CP.Field[1]; Log.Verbose(String(_dow));Log.Verbose(",");
      uint8_t _evnum=CP.Field[2]; Log.Verbose(String(_evnum));Log.Verbose(",");
      uint8_t _Tqua=CP.Field[3]; Log.Verbose(String(_Tqua));Log.Verbose(",");
      uint8_t _en=CP.Field[4]; Log.Verbose(String(_en));Log.Verbose(",");
      uint8_t _onoff=CP.Field[5]; Log.Verbose(String(_onoff));Log.Verbose(",");
      uint8_t _swn=CP.Field[6]; Log.Verbose(String(_swn));Log.Verbose("\n");
      Log.Info("\n#2:");
      Log.Info(HS.SetEventDay(_dow,_evnum,_Tqua,_en,_onoff,_swn));Log.Info("\n");
      Log.Info("#2:OK\n");
      //HS.WrEEPROMday(HS.Sched.WeekSched[_dow-1]);
    }
    break;
    case 3: //Save eventDay
    //format: #3,dow.
    if (CP.Nfield==2)
    {
      Log.Debug("\nCommand Store DayEvents ");
      uint8_t _dow=CP.Field[1]; Log.Verbose(String(_dow));Log.Verbose("\n");      
      HS.WrEEPROMday(HS.Sched.WeekSched[_dow-1]);
      Log.Info("\n#3:OK\n");
    }
    break;
    case 4: //Read alarm eventDay
    //format: #4,dow,evnum.
    if (CP.Nfield==3)
    {
      Log.Debug("\nCommand READ DayEvent: ");
      uint8_t _dow=CP.Field[1]; Log.Verbose(String(_dow));Log.Verbose(",");
      uint8_t _evnum=CP.Field[2]; Log.Verbose(String(_evnum));Log.Verbose("\n");
      HS.RdEEPROMday(_dow);
      Log.Info("\n#4:");
      Log.Info(HS.EventToStrLong(HS.Sched.WeekSched[_dow-1].Event[_evnum]));Log.Info("\n");   
      Log.Info("#4:OK\n");   
    }
    break;
    case 5: //Change Status Parameters
    //format: #5,Uartlev,ZBlev,Mode,ClockPer,TimeAdj.
    if (CP.Nfield==6)
    {
      Log.Debug("\nCommand Change Status Parameters: ");
      Log_en _Uartlev=CP.Field[1]; Log.Verbose(String(_Uartlev));Log.Verbose(",");
      Log_en _ZBlev=CP.Field[2]; Log.Verbose(String(_ZBlev));Log.Verbose(",");
      Mode_en _Mode=CP.Field[3]; Log.Verbose(String(_Mode));Log.Verbose(",");
      uint8_t _ClockPer=CP.Field[4]; Log.Verbose(String(_ClockPer));Log.Verbose(","); 
      int _TimeAdj=CP.Field[5]; Log.Verbose(String(_TimeAdj));Log.Verbose(",");
      HStat.ChangePar(_Uartlev,_ZBlev,_Mode,_ClockPer,_TimeAdj);      
      Log.Info("#5:OK\n");   
    }
    break;
    case 10: //Default Param
    //format: #10,mempart.     (mema part=0-->all)
    if (CP.Nfield==2)
    {
      Log.Debug("\nCommand EEPROM Default Param");
      if(CP.Field[1]==0){
          HS.ClearEEProm();
          HS.EEPromDefault();
          StatusCs S=StatusCs();
          S.SaveToEEProm();    
      }   
      Log.Info("\n#10:OK\n");
    }
    break;
    default:
     Log.Error("\n#Command Unknown\n");
  }
  return;
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Allarms
 */
 void Event0R() {
  //AlarmId id=  Alarm.getTriggeredAlarmId();
  HStat.Status.EvCalled=0;
  Log.Info("Triggered Alarm=");Log.Info(String(HStat.Status.EvCalled)); Log.Info("\n");
  //Serial.println(ev,DEC);
  int dow=weekday()-1;
  // use Alarm.free() to disable a timer and recycle its memory.

  if (HS.Sched.WeekSched[dow].Event[HStat.Status.EvCalled].EventCtrl.swTurn==swON)
  {
    TurnON();
  }
  else
  {
    TurnOFF();
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
  HStat.Status.EvCalled=1;
  Log.Info("Triggered Alarm=");Log.Info(String(HStat.Status.EvCalled)); Log.Info("\n");
  //Serial.println(ev,DEC);
  int dow=weekday()-1;
  // use Alarm.free() to disable a timer and recycle its memory.

  if (HS.Sched.WeekSched[dow].Event[HStat.Status.EvCalled].EventCtrl.swTurn==swON)
  {
    TurnON();
  }
  else
  {
    TurnOFF();
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
  HStat.Status.EvCalled=2;
  Log.Info("Triggered Alarm=");Log.Info(String(HStat.Status.EvCalled)); Log.Info("\n");
  //Serial.println(HStat.Status.EvCalled,DEC);
  int dow=weekday()-1;
  // use Alarm.free() to disable a timer and recycle its memory.

  if (HS.Sched.WeekSched[dow].Event[HStat.Status.EvCalled].EventCtrl.swTurn==swON)
  {
    TurnON();
  }
  else
  {
    TurnOFF();
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
  HStat.Status.EvCalled=3;
  Log.Info("Triggered Alarm=");Log.Info(String(HStat.Status.EvCalled)); Log.Info("\n");
  //Serial.println(HStat.Status.EvCalled,DEC);
  int dow=weekday()-1;
  // use Alarm.free() to disable a timer and recycle its memory.

  if (HS.Sched.WeekSched[dow].Event[HStat.Status.EvCalled].EventCtrl.swTurn==swON)
  {
    TurnON();
  }
  else
  {
    TurnOFF();
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
  HStat.Status.EvCalled=4;
  Log.Info("Triggered Alarm=");Log.Info(String(HStat.Status.EvCalled)); Log.Info("\n");
  //Serial.println(ev,DEC);
  int dow=weekday()-1;
  // use Alarm.free() to disable a timer and recycle its memory.

  if (HS.Sched.WeekSched[dow].Event[HStat.Status.EvCalled].EventCtrl.swTurn==swON)
  {
    TurnON();
  }
  else
  {
    TurnOFF();
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
  HStat.Status.EvCalled=5;
  Log.Info("Triggered Alarm=");Log.Info(String(HStat.Status.EvCalled)); Log.Info("\n");
  //Serial.println(ev,DEC);
  int dow=weekday()-1;
  // use Alarm.free() to disable a timer and recycle its memory.

  if (HS.Sched.WeekSched[dow].Event[HStat.Status.EvCalled].EventCtrl.swTurn==swON)
  {
    TurnON();
  }
  else
  {
    TurnOFF();
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
  HStat.Status.EvCalled=0;
  Log.Info("Triggered Alarm Single Event=");Log.Info(String(HStat.Status.EvCalled)); Log.Info("\n");
  //Serial.println(ev,DEC);
  
  // use Alarm.free() to disable a timer and recycle its memory.

  if (HS.Sched.EventFuture[HStat.Status.EvCalled].EventCtrl.swTurn==swON)
  {
    TurnON();
  }
  else
  {
    TurnOFF();
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
  HStat.Status.EvCalled=1;
  Log.Info("Triggered Alarm Single Event=");Log.Info(String(HStat.Status.EvCalled)); Log.Info("\n");
  //Serial.println(ev,DEC);
  
  // use Alarm.free() to disable a timer and recycle its memory.

  if (HS.Sched.EventFuture[HStat.Status.EvCalled].EventCtrl.swTurn==swON)
  {
    TurnON();
  }
  else
  {
    TurnOFF();
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
      return;
}

void UpdateSchedOnce()
{
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
}
// 
void TurnON()
{
  Log.Info("\nHeater ON\n");
}

void TurnOFF()
{
  Log.Info("\nHeater OFF\n");
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

