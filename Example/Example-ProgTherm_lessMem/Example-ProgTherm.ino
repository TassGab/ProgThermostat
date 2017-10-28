/*
   10/09/2017
   Gabriele Tasselli

   Scheduler for Programmable Thermostat
   weekly scheduler to be used with TimeAlarm library
   The schedule is stored in the eeprom
   For memory optimization the resolution is 15 minutes.
   Intially 4 events per day are allocated
*/
#include "CmdParserClass.h"
#include "Heater_scheduler_class.h"
#include "ProgThermClass.h"
#include <TimeAlarms.h>

AlarmId id0, id1, id2, id3, id4, id5, idse0, idse1;
//EventNum_sc ev; //Global var for event of the day
StatusCs HStat = StatusCs(); //Declare the status class
HeaterSchedulerCs HS = HeaterSchedulerCs(); //class instance
CmdParserClass CP = CmdParserClass();
int loop_c = 1;
String usbCommand = "";
int SwHeaters = 5;
//
void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);

  pinMode(SwHeaters, OUTPUT);
  digitalWrite(SwHeaters, LOW);
  //HS.ClearEEProm();
  if (HS.GetEEPromEmpty() == 0)
  {
    Log.Debug(F("\nEEPROM write default...\n"));
    HS.EEPromDefault();
    HStat.SaveToEEProm();
  }
  HStat.ReadFromEEProm();
#ifdef zigbee
  chibiInit();
#endif
  Log.Verbose(F("\nStatus:")); Log.Verbose(F("\n"));
  Log.Verbose(F("size=")); Log.Verbose(String(sizeof(HStat.Status), DEC)); Log.Verbose(F("\n"));
  Log.Verbose(F("mode=")); Log.Verbose(String(HStat.Status.Mode)); Log.Verbose(F("\n"));
  Log.Verbose(F("Loglev=")); Log.Verbose(String(HStat.Status.Uartlev)); Log.Verbose(F("\n"));
  Log.Verbose(F("ClockPeriod=")); Log.Verbose(String(HStat.Status.ClockPer)); Log.Verbose(F("\n"));
  setTime(23, 59, 50, 16, 10, 17); // set time
  //Alarm.alarmRepeat(0, 0, 0, UpdateMidnight); // update schedule at midnight
  UpdateSched();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0)
  {
    Log.Info(F("\nReceived a command\n"));
    //Serial.println("\nReceived a command\n"));
    usbCommand += Serial.readString();
    CP.Parse(usbCommand);
    usbCommand = "";
    if (CP.erFlag)
    {
      Log.Error(F("\nCommand Error\n"));
    }
    else
    {
      ExCommand(CP.Cmd);
    }
  }
#ifdef zigbee
  if (chibiDataRcvd() == true)
  {
    int len;
    byte buf[CHB_MAX_PAYLOAD];  // this is where we store the received data
    // retrieve the data and the signal strength
    len = chibiGetData(buf);
    // discard the data if the length is 0. that means its a duplicate packet
    if (len > 0)
    {
      Log.Info(F("\nReceived a command\n"));
      CP.Parse(String((char *)buf));
    }
  }
#endif
  Alarm.delay(1000); // wait one second between clock display
  if (loop_c++ == HStat.Status.ClockPer)
  {
    digitalClockDisplay();
    loop_c = 1;
  }
}//end loop
/***************************************
   Commands
*/
void ExCommand(uint8_t cmd)
{
  switch (cmd)
  {
    case 1: //set time
      //format: #1,hh,mm,ss,dd,mo,yy.
      if (CP.Nfield == 7)
      {

        Log.Debug(F("\nCommand set time: "));
        uint8_t hh = CP.Field[1]; Log.Verbose(String(hh)); Log.Verbose(F(":"));
        uint8_t mm = CP.Field[2]; Log.Verbose(String(mm)); Log.Verbose(F(":"));
        uint8_t ss = CP.Field[3]; Log.Verbose(String(ss)); Log.Verbose(F("\t"));
        uint8_t dd = CP.Field[4]; Log.Verbose(String(dd)); Log.Verbose(F("/"));
        uint8_t mo = CP.Field[5]; Log.Verbose(String(mo)); Log.Verbose(F("/"));
        uint8_t yy = CP.Field[6]; Log.Verbose(String(yy)); Log.Verbose(F("\n"));
        setTime(hh, mm, ss, dd, mo, yy);
        Log.Info(F("\n#1:OK\n"));
        //Log.Info(HS.TimeToStr(now()));
        //Log.Info("\n"));
      }
      else Log.Error(F("\n#1:Num. of field is wrong\n"));
      break;
    case 2: //set alarm eventDay
      //format: #2,dow,evnum,quarter,en,on/off,sw1.
      if (CP.Nfield == 7)
      {
        Log.Debug(F("\nCommand set DayEvent: "));
        uint8_t _dow = CP.Field[1]; Log.Verbose(String(_dow)); Log.Verbose(",");
        uint8_t _evnum = CP.Field[2]; Log.Verbose(String(_evnum)); Log.Verbose(",");
        uint8_t _Tqua = CP.Field[3]; Log.Verbose(String(_Tqua)); Log.Verbose(",");
        uint8_t _en = CP.Field[4]; Log.Verbose(String(_en)); Log.Verbose(",");
        uint8_t _onoff = CP.Field[5]; Log.Verbose(String(_onoff)); Log.Verbose(",");
        uint8_t _swn = CP.Field[6]; Log.Verbose(String(_swn)); Log.Verbose("\n");
        Log.Info("\n#2:");
        Log.Info(HS.SetEventDay(_dow, _evnum, _Tqua, _en, _onoff, _swn)); Log.Info(F("\n"));
        HS.WrEEPROMdayEv(HS.Sched.Daily);
        Log.Info(F("#2:OK\n"));
      }
      else Log.Error(F("\n#2:Num. of field is wrong\n"));
      break;
    case 3: //Read alarm eventDay
      //format: #3,dow,evnum.
      if (CP.Nfield == 3)
      {
        Log.Debug(F("\nCommand READ DayEvent: "));
        uint8_t _dow = CP.Field[1]; Log.Verbose(String(_dow)); Log.Verbose(",");
        uint8_t _evnum = CP.Field[2]; Log.Verbose(String(_evnum)); Log.Verbose(F("\n"));
        HS.RdEEPROMday(_dow, _evnum);
        Log.Info(F("\n#4:"));
        Log.Info(HS.EventToStrLong(HS.Sched.Daily.Event)); Log.Info(F("\n"));
        Log.Info(F("#4:OK\n"));
      }
      else Log.Error(F("\n#3:Num. of field is wrong\n"));
      break;
    case 4: //Read Status
      //format: #4.
      Log.Debug(F("\nCommand Read Status"));
       if (CP.Nfield == 1)
      {
        HStat.StatPrint();
        Log.Info(F("NextEvent at: "));Log.Info(HS.TimeToStr(Alarm.getNextTrigger()));Log.Info(F("\n"));
        digitalClockDisplay();
        Log.Info(F("#4:OK\n"));
      }
      else Log.Error(F("\n#4:Num. of field is wrong\n"));
    break;
    case 5: //Change Status Parameters
      //format: #5,Uartlev,ZBlev,Mode,ClockPer,TimeAdj.
      if (CP.Nfield == 6)
      {
        Log.Debug(F("\nCommand Change Status Parameters: "));
        Log_en _Uartlev = CP.Field[1]; Log.Verbose(String(_Uartlev)); Log.Verbose(",");
        Log_en _ZBlev = CP.Field[2]; Log.Verbose(String(_ZBlev)); Log.Verbose(",");
        Mode_en _Mode = CP.Field[3]; Log.Verbose(String(_Mode)); Log.Verbose(",");
        uint8_t _ClockPer = CP.Field[4]; Log.Verbose(String(_ClockPer)); Log.Verbose(",");
        int _TimeAdj = CP.Field[5]; Log.Verbose(String(_TimeAdj)); Log.Verbose(",");
        HStat.ChangePar(_Uartlev, _ZBlev, _Mode, _ClockPer, _TimeAdj);
        Log.Info(F("#5:OK\n"));
      }
      else Log.Error(F("\n#5:Num. of field is wrong\n"));
      break;
    case 6: //Change Mode
      //format: #6,Mode.
      if (CP.Nfield == 2)
      {
        Log.Debug(F("\nCommand Change Mode: "));
        Mode_en _Mode = CP.Field[1]; Log.Verbose(String(_Mode)); Log.Verbose(",");
        if (_Mode >= 0 and _Mode < 4)
        {
          HStat.NextState(_Mode);
          HStat.SaveToEEProm();
          UpdateSched();
        }
        Log.Info(F("#6:OK\n"));
      }
      else Log.Error(F("\n#6:Num. of field is wrong\n"));
      break;
    case 7: //Manual on/off
      //format: #7,on/off.
      if (CP.Nfield == 2)
      {
        Log.Debug(F("\nCommand Manualy Turn Heater: "));
        HStat.NextState(_Manual); //Set manual mode
        Heater_en _HeaterStat=CP.Field[1]; Log.Verbose(String(_HeaterStat)); Log.Verbose(F("\n"));
        if(_HeaterStat==_ON)
        {
          Log.Debug(F("ON\n"));
          TurnON();
        }
        else
        {
          Log.Debug(F("OFF\n"));
          TurnOFF();
        }
        Log.Info(F("#7:OK\n"));
      }
      else Log.Error(F("\n#7:Num. of field is wrong\n"));
      break;
    case 8: //set alarm Once
      //format: #8,quarter,gg,mo,yy,en,on/off.
      if (CP.Nfield == 7)
      {
        Log.Debug(F("\nCommand set Event Once: "));
        uint8_t _Tqua = CP.Field[1]; Log.Verbose(String(_Tqua)); Log.Verbose(",");
        uint8_t _Tgg = CP.Field[2]; Log.Verbose(String(_Tgg)); Log.Verbose(",");
        uint8_t _Tmo = CP.Field[3]; Log.Verbose(String(_Tmo)); Log.Verbose(",");
        uint8_t _Tyy = CP.Field[4]; Log.Verbose(String(_Tyy)); Log.Verbose(",");
        uint8_t _en = CP.Field[5]; Log.Verbose(String(_en)); Log.Verbose(",");
        uint8_t _onoff = CP.Field[6]; Log.Verbose(String(_onoff)); Log.Verbose(F("\n"));
//        uint8_t _swn = CP.Field[7]; Log.Verbose(String(_swn)); Log.Verbose("\n"));
        Log.Info("\n#8:");
        Log.Info(HS.SetEventOnce(0, (HS.SetTimeEvent(0, 0, 0, _Tgg, _Tmo, _Tyy) + _Tqua * 900), _en, _onoff, sw1)); Log.Info(F("\n"));
        HS. WrEEPROMevent(0, HS.Sched.EventFuture);
        Log.Info(F("#8:OK\n"));
      }
      else Log.Error(F("\n#8:Num. of field is wrong\n"));
      break;
    case 9: //Out of home for some hours
      //format: #9,nhours.
      if (CP.Nfield == 2)
      {
        Log.Debug(F("\nCommand Out of Home hours: "));
        uint8_t _hours=CP.Field[1]; Log.Verbose(String(_hours*SECS_PER_HOUR)); Log.Verbose(F("\n"));
        HStat.NextState(_Out); //Setmode out of home
        TurnOFF();
        id0 = Alarm.timerOnce(_hours, BackHome);
        HStat.SaveToEEProm();
        Log.Info(F("#9:OK\n"));
      }
      else Log.Error(F("\n#9:Num. of field is wrong\n"));
      break;
    case 10: //Default Param
      //format: #10,mempart.     (mema part=0-->all)
      if (CP.Nfield == 2)
      {
        Log.Info(F("\nCommand EEPROM Default Param"));
        if (CP.Field[1] == 0) {
          HS.ClearEEProm();
          HS.EEPromDefault();
          StatusCs S = StatusCs();
          S.SaveToEEProm();
        }
        Log.Info(F("\n#10:OK\n"));
      }
      else Log.Error(F("\n#10:Num. of field is wrong\n"));
      break;
    default:
      Log.Error(F("\n#Command Unknown\n"));
  }
  return;
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   Allarms
*/
void UpdateDailySched()
{
  Log.Info(F("Update Schedule\n"));
  int dow = weekday() - 1;
  Alarm.free(id0);
  // optional, but safest to "forget" the ID after memory recycled
  id0 = dtINVALID_ALARM_ID;
  if (!HS.GetNextDayEv(dow))
  {
    id0 = Alarm.timerOnce(HS.QuartToTime(HS.Sched.Daily.Event.EvTimeQ), Event0R);
    Log.Info(F("\n Next event: "));
    Log.Info(HS.EventToStrShort(HS.Sched.Daily.Event));
    Log.Info(F("\n"));
  }
  else
  {
    id0 = Alarm.alarmRepeat(0, 0, 0, UpdateDailySched); // update schedule at midnight
    Log.Info(F("\nNext Event: Midnight\n"));
  }
  return;
}

void Event0R() {
  //AlarmId id=  Alarm.getTriggeredAlarmId();
  //HStat.Status.EvCalled = 0;
  Log.Info(F("\nTriggered Alarm: dow=")); Log.Info(String(HS.Sched.Daily.dow)); Log.Info(F(", ev num="));Log.Info(String(HS.Sched.Daily.EvNum)); Log.Info(F("\n"));
  if (HStat.Status.Mode == _Daily)
  {
    if (HS.Sched.Daily.Event.EventCtrl.swTurn == swON)
    {
      TurnON();
    }
    else
    {
      TurnOFF();
    }
  }
  UpdateDailySched();
  return;
}

void EventOnce0() {
  //AlarmId id=  Alarm.getTriggeredAlarmId();
  //HStat.Status.EvCalled = 0;
  Log.Info(F("\nTriggered Alarm Single Event")); Log.Info(F("\n"));
  //Serial.println(ev,DEC);

  // use Alarm.free() to disable a timer and recycle its memory.
  if (HStat.Status.Mode == _Once)
  {
    if (HS.Sched.EventFuture.EventCtrl.swTurn == swON)
    {
      TurnON();
    }
    else
    {
      TurnOFF();
    }
  }
  Alarm.free(idse0);
  // optional, but safest to "forget" the ID after memory recycled
  idse0 = dtINVALID_ALARM_ID;
  // you can also use Alarm.disable() to turn the timer off, but keep
  // it in memory, to turn back on later with Alarm.enable().
  return;
}
void BackHome()
{
  Log.Info(F("\nTriggered Event: BackHome\n"));
  TurnON();
  HStat.BackLastState();
  HStat.SaveToEEProm();
  UpdateSched();
  return;
}

void UpdateSchedOnce()
{
  HS.RdEEPROMevent(0);
  if (HS.Sched.EventFuture.EventCtrl.isEnabled == evEN)
  {
    idse0 = Alarm.triggerOnce(HS.Sched.EventFuture.TimeEv, EventOnce0);
    Log.Info(F("\nidse0: "));
    Log.Info(HS.EventOnceToStrShort(HS.Sched.EventFuture));
    Log.Info(F("\n"));
  }
}
//-----------------------------------------
void UpdateSched()
{
  if(HStat.Status.Mode==_Daily) UpdateDailySched();
  else if(HStat.Status.Mode==_Once) UpdateSchedOnce();
  return;
}
void TurnON()
{
  Log.Info(F("\nHeater ON\n"));
  digitalWrite(SwHeaters, HIGH);
  HStat.Status.HeaterStatus=_ON;
  HStat.SaveToEEProm();
}

void TurnOFF()
{
  Log.Info(F("\nHeater OFF\n"));
  digitalWrite(SwHeaters, LOW);
  HStat.Status.HeaterStatus=_OFF;
  HStat.SaveToEEProm();
}
void digitalClockDisplay() {
  // digital clock display of the time
  Log.Info(String(hour()));
  printDigits(minute());
  printDigits(second());
  Log.Info(F("\n"));
  return;
}

void printDigits(int digits) {
  Log.Info(F(":"));
  if (digits < 10)
    Log.Info(F("0"));
  Log.Info(String(digits));
  return;
}

