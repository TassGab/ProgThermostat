#ifndef HeaterSched_h
#define HeaterSched_h

#include "Logging_class.h"
#include "TimeLib.h"
#include <Arduino.h>
#include <EEPROM.h>
#define nEventPerDay 6 
//#define nEventFuture 1

//#define debug
//Macro
//#ifndef debug
// #define QuartToTime(_quarter_) (_quarter_ * SECS_PER_MIN * 15UL+5UL)   //Calc the seconds from the quarter hours
//#endif
#define TimeToQuart(_time_) (((_time_ % SECS_PER_DAY) / SECS_PER_MIN)/15UL)
typedef uint8_t TimeQuarter_sc;
enum EventState_sc:bool {evEN=true, evDIS=false} ; 
enum SwitchState_sc:bool {swON=true, swOFF=false} ; 
enum SwitchNum_sc:uint8_t {sw1=1, sw2=2} ;

typedef struct
{
  EventState_sc isEnabled ; //Enable=1 disable=0
  SwitchNum_sc nSwitch; //Switch 1=0 Switch 2=1
  SwitchState_sc swTurn; //State=0 --> turn off, State=1 --> turn ON
} EventCtrl_sc;
typedef struct 
{  
  TimeQuarter_sc EvTimeQ; //Time of the event in quarter of hour
  EventCtrl_sc EventCtrl; //Event control (ON OFF, En Dis, switch to turn
}EventType_sc;
typedef struct
{
  timeDayOfWeek_t dow; // day of the week
  EventType_sc Event[nEventPerDay]; //array of events per day
} DaySchedule_sc;
typedef struct
{
  time_t TimeEv;
  EventCtrl_sc EventCtrl;
} EventFuture_sc;

typedef struct
{
  DaySchedule_sc Daily; //Array of DaySchedule_sc representing the week schedule
  EventFuture_sc EventFuture; //Event once in a certain time
  uint8_t EEPROMempty;
} Sched_sc;
class HeaterSchedulerCs
{
  public:
    Sched_sc Sched;
    HeaterSchedulerCs();    
    String SetEventDay(timeDayOfWeek_t, uint8_t, TimeQuarter_sc, EventState_sc, SwitchState_sc, SwitchNum_sc);
    String SetEventOnce(uint8_t, time_t, EventState_sc, SwitchState_sc, SwitchNum_sc);
    String TimeToStr(time_t);
    time_t SetTimeOfDay(uint8_t, uint8_t, uint8_t);
    time_t QuartToTime(TimeQuarter_sc);
    time_t SetTimeEvent(uint8_t, uint8_t, uint8_t,uint8_t, uint8_t, uint8_t);
    String EventToStrShort(EventType_sc);
    String EventOnceToStrShort(EventFuture_sc); 
    String EventToStrLong(EventType_sc);
    void RdEEPROMday(timeDayOfWeek_t);
    void RdEEPROMevent(uint8_t);
    void WrEEPROMday(DaySchedule_sc);
    void WrEEPROMevent(uint8_t,EventFuture_sc);
    void EEPromDefault();
    uint8_t GetEEPromEmpty();
    void ClearEEProm();
  private:
    int GetEEoffAddress=30;
    int GetEEdaystructsize=0;
    int GetEEeventstructsize=0;
    
    DaySchedule_sc  DayDeepCp(DaySchedule_sc); 
    EventFuture_sc  EvDeepCp(EventFuture_sc);
    
};
#endif
