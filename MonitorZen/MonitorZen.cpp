// MonitorZen.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MonitorZen.h"
#include <string>
#include <iostream>
#include <set>
#include <unordered_map>
#include <map>
#include <vector>
#include <utility>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
bool screening;									// true when blacked out
std::vector<HWND> currentScreens;

// ASSUMPTION: the indicies of these two maps are linked together;
//	each map is sorted: by compare_1() or default.
// moninfo:mon_handles
std::map<MONITORINFO, HMONITOR, compare_1> MoninfoToHmonMap; 
// IDC_CHECKBOX offset:checkbox_handles 
std::map<int, HWND> OffsetsToHwndMap;			

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
HWND addMainButton(HWND &hWnd, HINSTANCE &hInstance);
HWND addCheckboxes(HWND &hWnd, HINSTANCE &hInstance);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MONITORZEN, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	RegisterScreen(hInstance);

	// Grab #monitors; initializes MoninfoToHmonMap.
	// todo: Consider passing in the map as param; more clarity?
	MonitorCount();

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MONITORZEN));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MONITORZEN));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(255, 255, 255)); /*(HBRUSH)GetStockObject(COLOR_WINDOW + 1);*/
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MONITORZEN);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//  FUNCTION: RegisterScreen(HINSTANCE hInstance)
//
//  PURPOSE: Registers windows used to cover selected monitors.
//
ATOM RegisterScreen(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProcScreen;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MONITORZEN));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0)); /*(HBRUSH)GetStockObject(COLOR_WINDOW + 1);*/
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MONITORZEN);
	wcex.lpszClassName = L"screen";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	// create main window
	HWND hWnd = CreateWindowW(szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW & (~WS_THICKFRAME) & (~WS_MINIMIZEBOX) & (~WS_MAXIMIZEBOX),
		CW_USEDEFAULT,
		0,
		400,
		400,
		nullptr, nullptr, hInstance, nullptr);

	
	addMainButton(hWnd, hInstance);
	
	addCheckboxes(hWnd, hInstance);

	if (!hWnd) { return FALSE; }
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//   PURPOSE: Adds button used to create black overlays over each flagged 
//      monitor.
// 
//   RETURN: returns NULL if failure to create window; otherwise, return
//      handle of button.
HWND addMainButton(HWND &hWnd, HINSTANCE &hInstance)
{
	HWND hWndButton = CreateWindow(
		L"BUTTON",
		L"Cover Monitors",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_FLAT,
		133 + 12,
		300,
		105,
		30,
		hWnd,
		(HMENU)IDC_ACTIVATE_SCREEN,
		hInstance,
		NULL);      
	return hWndButton;
}

//   PURPOSE: Adds one checkbox to the main application window for every 
//      detected monitor. Labels are named accordingly to monitor number.
// 
//   RETURN: returns NULL if failure to create window; otherwise, return
//      handle of button.
HWND addCheckboxes(HWND &hWnd, HINSTANCE &hInstance)
{
	// vars to create a checkbox
	long offset = 0;
	HWND hCheckBox;
	wchar_t label[256];
	
	// iteratively create a checkbox for each monitor;
	//		position immediately below one another.
	for (unsigned i = 0; i < MoninfoToHmonMap.size(); ++i)
	{
		wsprintfW(label, L"Monitor %d", offset + 1); // 1-based label numbering
		hCheckBox = CreateWindow(L"button", label,
			WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
			20, 20 + i * 35, 185, 35,
			hWnd, (HMENU)(IDC_CHECKBOX + offset), hInstance, 0);

		// insert into global map
		OffsetsToHwndMap.insert(std::pair<int, HWND >(offset, hCheckBox));
		offset++;
	}
	
	if (MoninfoToHmonMap.size() != OffsetsToHwndMap.size())
	{
		hCheckBox = NULL;
	}
	return hCheckBox;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		BOOL checked;

		// Parse the menu selections:
		if (wmId >= IDC_CHECKBOX)
		{
			checked = IsDlgButtonChecked(hWnd, wmId);
			if (checked) {
				CheckDlgButton(hWnd, wmId, BST_UNCHECKED);
			}
			else {
				CheckDlgButton(hWnd, wmId, BST_CHECKED);
			}
			break;
		}

		switch (wmId)
		{
		case IDC_ACTIVATE_SCREEN: // handle main button press
			(!screening) ? (CreateOverlays(hWnd, 0), screening = true) : (deleteOverlays(hWnd), screening = false);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


LRESULT CALLBACK WndProcScreen(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);

		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		break;
	}
	return (INT_PTR)FALSE;
}

BOOL CreateOverlays(HWND hWnd, int nCmdShow)
{
	// TODO: Create a check that deletes overlays if they already exist
	/* code goes here */
	int topLeftX = 0; int topLeftY = 0;
	int botRightX = 0; int botRightY = 0;
	MONITORINFO info;
	HWND hWndScreen;
	UINT checked = false;

	// Two iterators are used to simultaneously iterate over two maps of the
	//	the same size. The indices on each map both correspond to a monitor.
	auto MoninfoIt = MoninfoToHmonMap.begin();
	auto offsetsIt = OffsetsToHwndMap.begin();
	for (; MoninfoIt != MoninfoToHmonMap.end(); ++MoninfoIt, ++offsetsIt)
	{
		// check if the current checkbox is checked
		checked = IsDlgButtonChecked(hWnd, IDC_CHECKBOX + offsetsIt->first);
		if (checked == BST_CHECKED)
		{
			// initialize info from MoninfoToHmonMap
			info = MoninfoIt->first;
			topLeftX = info.rcMonitor.left;
			topLeftY = info.rcMonitor.top;
			botRightX = info.rcMonitor.right;
			botRightY = info.rcMonitor.bottom;

			// create black screen on correct location
			hWndScreen = CreateWindowW(L"screen",
				L"Meme",
				WS_OVERLAPPEDWINDOW & (~WS_THICKFRAME) & (~WS_MINIMIZEBOX) & (~WS_MAXIMIZEBOX),
				topLeftX,
				topLeftY,
				botRightX - topLeftX,
				botRightY - topLeftY,
				nullptr, nullptr, nullptr, nullptr);

			currentScreens.push_back(hWndScreen);

			SetWindowLong(hWndScreen, GWL_STYLE, 0); // removes title bar
			SetMenu(hWndScreen, NULL); // removes menu bar

			ShowWindow(hWndScreen, SW_SHOWDEFAULT);
			UpdateWindow(hWndScreen);
		}
		
	}

	return TRUE;
}

BOOL deleteOverlays(HWND hWnd)
{
	HWND currentWindow;
	while (!currentScreens.empty())
	{
		currentWindow = currentScreens.back();
		DestroyWindow(currentWindow);
		currentScreens.pop_back();
	}

	return true;
}

// Helps enumerate monitors and populates 
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	// Initialize and store MONITORINFO globally
	MONITORINFO info;
	info.cbSize = sizeof(info); // req
	GetMonitorInfo(hMonitor, &info);
	MoninfoToHmonMap.insert(std::pair<MONITORINFO, HMONITOR>(info, hMonitor));

	int *Count = (int*)dwData;
	(*Count)++;
	return TRUE;
}

int MonitorCount()
{
	int Count = 0;
	if (EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&Count))
		return Count;
	return -1;		//signals an error
}


