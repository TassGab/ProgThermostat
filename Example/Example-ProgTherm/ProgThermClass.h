#ifndef _ProgClass
#define  _ProgClass
#include <EEPROM.h>
#include "Logging_class.h"

typedef enum {_Manual, _Daily, _Once, _Out} Mode_en;
typedef enum {_ON,_OFF} Heater_en;
typedef enum {Event0, Event1, Event2, Event3, Event4, Event5} EventNum_sc;
typedef struct 
{
  Log_en Uartlev; //log level for uart
  Log_en ZBlev; //log level for ZigBee
  Mode_en Mode; //Mode of operation
  uint8_t ClockPer; //period of clock visualization in secs.
  int TimeAdj; //time adjust
  Heater_en HeaterStatus; //Status of heater
  EventNum_sc EvCalled; //Event number called
  Mode_en LastState;
}HeaterStatusSt;
class StatusCs
{
  public:
  HeaterStatusSt Status;
  StatusCs();
  void NextState(Mode_en);
  void BackLastState();
  void ReadFromEEProm(); 
  void SaveToEEProm();
  void ChangePar(Log_en ,Log_en ,Mode_en ,uint8_t , int );
  private:  
  int GetEEoffAddress=0;
  
};
#endif
