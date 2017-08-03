# MonitorZen
A simple application to rid of visual distractions by creating black overlays 
over selected monitors. 

This program only works on the Windows OS. Written using WINAPI in C++. 
Target the x86 platform when building. 

Designed to mimic the zen mode in IDEs, MonitorZen removes from focus the 
distracting windows, notifications, and taskbar that might be present on
your non-dominant monitor. With a single hotkey you can block out everything and
focus on reading your article at hand. 

# In Action



# Files of Note
### MonitorZen.h
Includes forward declarations of functions involved in registering and 
constructing the main window UI and screen overlays.
### MonitorZen.cpp
Entry point to the program. All functions, including wMain, UI creation, 