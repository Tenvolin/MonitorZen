#pragma once
#include <utility>
#include "resource.h"

// Forward declarations of functions included in this code module:
// Code for construction of main window: buttons and checkboxes. Helpers too.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
HWND addMainButton(HWND &hWnd, HINSTANCE &hInstance);
HWND addCheckboxes(HWND &hWnd, HINSTANCE &hInstance);

// code for construction of black overlay windows. Also includes helpers.
ATOM RegisterScreen(HINSTANCE hInstance);
LRESULT CALLBACK WndProcScreen(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
void toggleOverlays(HWND hWnd); 
BOOL CreateOverlays(HWND hWnd, int nCmdShow);
BOOL deleteOverlays(HWND hWnd);
int MonitorCount();

// TODO: Refactor; place somewhere nicer?
class compare_1
{
public:
	bool operator()(const MONITORINFO &x, const MONITORINFO &y)
	{
		return x.rcMonitor.left < y.rcMonitor.left;
	}
};