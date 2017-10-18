#ifndef HeaterSched_h
#define HeaterSched_h

#include<Logging_class.h>
#include "TimeLib.h"
#include <Arduino.h>
#include <EEPROM.h>
#define nEventPerDay 6 
#define nEventFuture 2

//Macro
//#define QuartToTime(_quarter_) (_quarter_ * SECS_PER_MIN * 15UL)
#define TimeToQuart(_time_) (((_time_ % SECS_PER_DAY) / SECS_PER_MIN)/15UL)
typedef uint8_t TimeQuarter_sc;
enum EventState_sc:bool {evEN=true, evDIS=false} ; 
enum SwitchState_sc:bool {swON=true, swOFF=false} ; 
enum SwitchNum_sc:uint8_t {sw1=1, sw2=2} ;
typedef enum {Event0, Event1, Event2, Event3, Event4, Event5} EventNum_sc;
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
  DaySchedule_sc WeekSched[7]; //Array of DaySchedule_sc representing the week schedule
  EventFuture_sc EventFuture[nEventFuture]; //Event once in a certain time
} Sched_sc;
class HeaterSchedulerCs
{
  public:
    Sched_sc Sched;
    HeaterSchedulerCs();    
    String SetEventDay(timeDayOfWeek_t, EventNum_sc, TimeQuarter_sc, EventState_sc, SwitchState_sc, SwitchNum_sc);
    String SetEventOnce(EventNum_sc, time_t, EventState_sc, SwitchState_sc, SwitchNum_sc);
    String TimeToStr(time_t);
    time_t SetTimeOfDay(uint8_t, uint8_t, uint8_t);
    time_t QuartToTime(TimeQuarter_sc);
    time_t SetTimeEvent(uint8_t, uint8_t, uint8_t,uint8_t, uint8_t, uint8_t);
    String EventToStrShort(EventType_sc);
    String EventOnceToStrShort(EventFuture_sc); 
    void RdEEPROMday(timeDayOfWeek_t);
    void RdEEPROMevent(uint8_t);
    void WrEEPROMday(DaySchedule_sc);
    void WrEEPROMevent(uint8_t,EventFuture_sc);
    void EEPromDefault();
    
  private:
    int GetEEoffAddress=0;
    int GetEEdaystructsize=0;
    int GetEEeventstructsize=0;
    String EventToStrLong(EventType_sc);
    void DayDeepCp(DaySchedule_sc, DaySchedule_sc); 
    void EvDeepCp(EventFuture_sc,EventFuture_sc);
    
};
#endif
