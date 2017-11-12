#ifndef _ProgClass
#define  _ProgClass
#include <EEPROM.h>
#include "Logging_class.h"

typedef enum {_Manual, _Daily, _Once, _Out} Mode_en;
typedef enum Heater_en: bool{_OFF,_ON} ;
typedef enum {Event0, Event1, Event2, Event3, Event4, Event5} EventNum_sc;
typedef struct 
{
  Log_en Uartlev; //log level for uart
  Log_en ZBlev; //log level for ZigBee
  Mode_en Mode; //Mode of operation
  uint8_t ClockPer; //period of clock visualization in secs.
  int TimeAdj; //time adjust
  //Heater_en HeaterStatus; //Status of heater
  //EventNum_sc EvCalled; //Event number called
  Mode_en LastState;
}HeaterStatusSt;
class StatusCs
{
  public:
  HeaterStatusSt Status;  
  Heater_en State; //Status of heater ON OFF
  StatusCs(); 
  void NextState(Mode_en);
  void BackLastState();
  HeaterStatusSt ReadFromEEProm(); 
  void SaveToEEProm(HeaterStatusSt);
  void ChangePar(Log_en ,Log_en ,Mode_en ,uint8_t , int );
  void StatPrint();
  private:  
  int GetEEoffAddress=0;

  String ModeToStr(Mode_en);  
};
#endif
