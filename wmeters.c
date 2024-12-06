/*
 * wmeters.c
 *
 *  Created on: May 5, 2020
 *      Author: Patrick-Gilles Maillot
 *
 */
#include <windows.h>
#include <stdio.h>
#include <sys/time.h>
#include "wapi.h"
#include "wext.h"

// Windows Declarations
WINBASEAPI HWND WINAPI GetConsoleWindow(VOID);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
HINSTANCE		hInstance = 0;
HWND			hwndipaddr, hwndconx;
HDC				hdc;
PAINTSTRUCT		ps;
MSG				wMsg;
HFONT			hfont;
HPEN			wlpen, wnpen;			// full line, no line
HBRUSH			gBrush, yBrush, rBrush, wBrush;	// green, yellow, red, white
int				keep_running = 1;	// mainloop control
int				ready = 0;			// Ready flag after connect OK
char			wingip[24] = "";	// Let wapi tell us our IP
int				M_id = 3;			// Meters request ID
int				M_port = 10026;		// Meters UDP port

#define MAXLEN	128
unsigned char   buf[MAXLEN];		// data buffer
int				len;

time_t			before = 0;			// Timers
time_t			now;

unsigned char	mbits[29] = {0x80, 0, 0, 0, 0x01}; // channel 1 &
                                                   // channel 40


//
// void wDrawMeter()
// A basic vu-meter, positioned at basex, basey, height is 128, width is 15
// value is the current vu position
void wDrawMeter(int basex, int basey, int value) {
	int basexx = basex + 15;
	int baseyy = basey + 128;
	// erase
	SelectObject(hdc, wBrush);
	SelectObject(hdc, wlpen);
	Rectangle(hdc, basex-2, baseyy+2, basexx+2, basey-2);
	Rectangle(hdc, basex, baseyy, basexx, basey);
	basex++;
	// values to 90 are green
	// values above 120 are red
	// yellow in between
	SelectObject(hdc, gBrush);
	SelectObject(hdc, wnpen);
	if (value < 90) {
		Rectangle(hdc, basex, baseyy, basexx, baseyy - value);
	} else if (value < 120) {
		Rectangle(hdc, basex, baseyy, basexx, baseyy - 90);
		SelectObject(hdc, yBrush);
		Rectangle(hdc, basex, baseyy - 90, basexx, baseyy - value);
	} else {
		Rectangle(hdc, basex, baseyy, basexx, baseyy - 90);
		SelectObject(hdc, yBrush);
		Rectangle(hdc, basex, baseyy - 90, basexx, baseyy - 120);
		SelectObject(hdc, rBrush);
		Rectangle(hdc, basex, baseyy - 120, basexx, baseyy - value);
	}
}
//
// Windows main function and main loop
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdFile) {
//
	union {
		unsigned char	cc[2];
		short			ii;
	} endian;
	int					ival = 0;
	//
	WNDCLASSW wc = {0};
	wc.lpszClassName = L"wapi demo";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	//
	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName, L"wmeters - WING meters",
		WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU,
		100, 220, 150, 250, 0, 0, hInstance, 0);
	//
	// Main loop
	while (keep_running) {
		if (PeekMessage(&wMsg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&wMsg);
			DispatchMessage(&wMsg);
		}
		if (ready) {
			now = time(NULL);				// maintain meters
			if (now > before + 4) {			// by sending
				wRenewMeters(M_id);			// request every less than
				before = now;				// 5 seconds
			}
			// Read meters (if any data) with a timeout of 10ms
			if ((len = wGetMeters(buf, MAXLEN, 10000)) > 0) {
				endian.cc[0] = buf[5];			// channel in 1
				endian.cc[1] = buf[4];
				ival = endian.ii / 256 + 128;	// simple linear scale
				wDrawMeter(10, 70, ival);
				endian.cc[0] = buf[9];			// channel out
				endian.cc[1] = buf[8];
				ival = endian.ii / 256 + 128;	// simple linear scale
				wDrawMeter(35, 70, ival);
				endian.cc[0] = buf[25];			// channel out 40
				endian.cc[1] = buf[24];
				ival = endian.ii / 256 + 128;	// simple linear scale
				wDrawMeter(100, 70, ival);
			}
		}
	}
	return (int) wMsg.wParam;
}
//
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	//
	char str1[16];

	switch (msg) {
	case WM_CREATE:
		hwndipaddr = CreateWindow("Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
				5, 40, 120, 20, hwnd, (HMENU)0, NULL, NULL);
		hwndconx = CreateWindow("button", "Connect", WS_VISIBLE | WS_CHILD,
				5, 15, 85, 20, hwnd, (HMENU)1, NULL, NULL);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		SelectObject(hdc, hfont);
		TextOut(hdc, 6, 200, str1, wsprintf(str1, "CH01"));
		TextOut(hdc, 31, 200, str1, wsprintf(str1, "CH01"));
		TextOut(hdc, 96, 200, str1, wsprintf(str1, "CH40"));
		break;
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {	// user action
			switch (LOWORD(wParam)) {
			case 1:
				if (ready) {
					keep_running = 0;
					PostQuitMessage(0);
				} else {
					// Connect clicked
					if (wOpen(wingip) != WSUCCESS) exit(1);
					SetWindowText(hwndipaddr, wingip);
					// set udp  port to receive UDP data and request meters for channel 1
					if (wMeterUDPPort(M_port) != WSUCCESS) exit(1);
					if (wSetMetersRequest(M_id, mbits) != WSUCCESS) exit(1); // chanel 1; Meter req ID 3
					ready = 1;
				}
				break;
			}
		}
		break;
	case WM_DESTROY:
		keep_running = 0;
		PostQuitMessage(0);
		break;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}
//
//
// main program
int main(int argc, char **argv) {
	HINSTANCE hPrevInstance = 0;
	PWSTR pCmdLine = 0;
	int nCmdFile = 0;
	// Hide console window
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	// Set colors
	gBrush = CreateSolidBrush(RGB(30, 255, 30));
	yBrush = CreateSolidBrush(RGB(255, 255, 30));
	rBrush = CreateSolidBrush(RGB(2255, 30, 30));
	wBrush = CreateSolidBrush(RGB(255, 255, 255));
	wlpen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	wnpen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
	hfont = CreateFont(12, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
	        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
	        ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
	// Launch program
	wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdFile);
	wClose();
	return 0;
}
