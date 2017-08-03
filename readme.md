# MonitorZen
A simple application to rid of visual distractions by creating black overlays 
over selected monitors. 

This program only works on the Windows OS. Written using WINAPI in C++. 
Target the x86 platform when building. 

Designed to mimic the zen mode in IDEs, MonitorZen removes from focus the 
distracting windows, notifications, and taskbar that might be present on
your non-dominant monitor. With a single hotkey you can block out everything and
focus on reading your article at hand. 

# How it Works
The program works by painting the client area back in each child window. 
Each of these windows are positioned to overlay the resolution of each selected
monitor. 

The title bar and menu bar are removed, thus painting a complete black
overlay on each selected monitor. Each child window also contains styles that 
keep them permanently above all other windows.

# In Action
![Usage](https://github.com/Tenvolin/MonitorZen/blob/master/images/usage.gif)
![Main Window](https://github.com/Tenvolin/MonitorZen/blob/master/images/mainwindow.png)

# To Use
### Hotkey: ctrl + ~
Start the program and a checkbox will appear for each monitor you have in use.
The monitors are numbered from 1 onwards, counting from your leftmost monitor 
first. When you want to black out certain overlays, press the hotkeys "ctrl"+"~"
or click on the main bottom.

If you forget about the hotkeys, you can always "alt"+"f4" out of these overlays.

# Files of Note
### MonitorZen.h
Includes forward declarations of functions involved in registering and 
constructing the main window UI and screen overlays.
### MonitorZen.cpp
Entry point to the program. All functions and message handling loop defined here.