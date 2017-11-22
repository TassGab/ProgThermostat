# Progammable Scheduler for Heaters control with Arduino Chibi (ZigBee)
This repository contains the arduino code to implement a flexible scheduler for the heaters control.
The arduino board needs a simple shield with a relay or solid state relay to be connected in series with the thermostat line.
The scheduler can be placed close to the Boiler and remotely controlled by ZigBee interface.
Functionalities:
-Weekly scheduler with 6 events per day. Time resolution 15 minutes.
-Single event happening one time in the future (ex: heating up the house just before coming back from vacation)
-Manual control
-Function "out of home": you can turn off the heaters for some hours and then go back to the weekly schedule

The repository shows a GUI for setup and control. The GUI is written in C#.NET
