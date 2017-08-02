#pragma once

#include "resource.h"
BOOL CreateOverlays(HINSTANCE hInstance, int nCmdShow);
ATOM RegisterScreen(HINSTANCE hInstance);
LRESULT CALLBACK WndProcScreen(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
int MonitorCount();

int compare(const void * a, const void * b)
{
	MONITORINFO *am; MONITORINFO *bm;
	am = (MONITORINFO*)a;
	bm = (MONITORINFO*)b;


	;
	return am->rcMonitor.left - bm->rcMonitor.left;
}