#pragma once

#include "resource.h"
BOOL CreateOverlays(HINSTANCE hInstance, int nCmdShow);
ATOM RegisterScreen(HINSTANCE hInstance);
LRESULT CALLBACK WndProcScreen(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);