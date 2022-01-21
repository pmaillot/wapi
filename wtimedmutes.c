/*
 * wtimedmutes.c
 *
 *  Created on: Mar 1, 2021
 *      Author: Patrick-Gilles Maillot
 *      A quick demo of timed controlled audio muting...
 *
 *
 *      v 0.1: simple version (concept)
 *      v 1.0: extending to all channel strips
 *      v 2.0: added threads to enable each mute to take place independently from others
 *             added parameters to control level change slope (dB, time, accel)
 *      v 3.0  added Windows GUI (March 6, 2021)
 *      v 3.1  corrected possible memory leak
 */
//
#include <unistd.h>
#include <stdio.h>
#include <string.h>
//
#include "../wapi/wapi.h"
#include "../wapi/wext.h"
#include "WRRIcon.h"
//
#ifdef __WIN32__
#include <windows.h>
#define millisleep(a)	Sleep(a)
#else
#define millisleep(a)	usleep((a)*1000)
typedef unsigned short	boolean;
#endif
//
//
wtoken	mutes[] = {CH_1_MUTE, CH_2_MUTE, CH_3_MUTE, CH_4_MUTE, CH_5_MUTE, CH_6_MUTE, CH_7_MUTE, CH_8_MUTE,
					CH_9_MUTE, CH_10_MUTE, CH_11_MUTE, CH_12_MUTE, CH_13_MUTE, CH_14_MUTE, CH_15_MUTE, CH_16_MUTE,
					CH_17_MUTE, CH_18_MUTE, CH_19_MUTE, CH_20_MUTE, CH_21_MUTE, CH_22_MUTE, CH_23_MUTE, CH_24_MUTE,
					CH_25_MUTE, CH_26_MUTE, CH_27_MUTE, CH_28_MUTE, CH_29_MUTE, CH_30_MUTE, CH_31_MUTE, CH_32_MUTE,
					CH_33_MUTE, CH_34_MUTE, CH_35_MUTE, CH_36_MUTE, CH_37_MUTE, CH_38_MUTE, CH_39_MUTE, CH_40_MUTE,
					AUX_1_MUTE, AUX_2_MUTE, AUX_3_MUTE, AUX_4_MUTE, AUX_5_MUTE, AUX_6_MUTE, AUX_7_MUTE, AUX_8_MUTE,
					BUS_1_MUTE, BUS_2_MUTE, BUS_3_MUTE, BUS_4_MUTE, BUS_5_MUTE, BUS_6_MUTE, BUS_7_MUTE, BUS_8_MUTE,
					BUS_9_MUTE, BUS_10_MUTE, BUS_11_MUTE, BUS_12_MUTE, BUS_13_MUTE, BUS_14_MUTE, BUS_15_MUTE, BUS_16_MUTE,
					MAIN_1_MUTE, MAIN_2_MUTE, MAIN_3_MUTE, MAIN_4_MUTE,
					MTX_1_MUTE, MTX_2_MUTE, MTX_3_MUTE, MTX_4_MUTE, MTX_5_MUTE, MTX_6_MUTE, MTX_7_MUTE, MTX_8_MUTE,
					DCA_1_MUTE, DCA_2_MUTE, DCA_3_MUTE, DCA_4_MUTE, DCA_5_MUTE, DCA_6_MUTE, DCA_7_MUTE, DCA_8_MUTE,
					DCA_9_MUTE, DCA_10_MUTE, DCA_11_MUTE, DCA_12_MUTE, DCA_13_MUTE, DCA_14_MUTE, DCA_15_MUTE, DCA_16_MUTE};
//
#define MNUM sizeof(mutes) / sizeof(wtoken)
//
wtoken	fader[MNUM] = {CH_1_FDR, CH_2_FDR, CH_3_FDR, CH_4_FDR, CH_5_FDR, CH_6_FDR, CH_7_FDR, CH_8_FDR,
					CH_9_FDR, CH_10_FDR, CH_11_FDR, CH_12_FDR, CH_13_FDR, CH_14_FDR, CH_15_FDR, CH_16_FDR,
					CH_17_FDR, CH_18_FDR, CH_19_FDR, CH_20_FDR, CH_21_FDR, CH_22_FDR, CH_23_FDR, CH_24_FDR,
					CH_25_FDR, CH_26_FDR, CH_27_FDR, CH_28_FDR, CH_29_FDR, CH_30_FDR, CH_31_FDR, CH_32_FDR,
					CH_33_FDR, CH_34_FDR, CH_35_FDR, CH_36_FDR, CH_37_FDR, CH_38_FDR, CH_39_FDR, CH_40_FDR,
					AUX_1_FDR, AUX_2_FDR, AUX_3_FDR, AUX_4_FDR, AUX_5_FDR, AUX_6_FDR, AUX_7_FDR, AUX_8_FDR,
					BUS_1_FDR, BUS_2_FDR, BUS_3_FDR, BUS_4_FDR, BUS_5_FDR, BUS_6_FDR, BUS_7_FDR, BUS_8_FDR,
					BUS_9_FDR, BUS_10_FDR, BUS_11_FDR, BUS_12_FDR, BUS_13_FDR, BUS_14_FDR, BUS_15_FDR, BUS_16_FDR,
					MAIN_1_FDR, MAIN_2_FDR, MAIN_3_FDR, MAIN_4_FDR,
					MTX_1_FDR, MTX_2_FDR, MTX_3_FDR, MTX_4_FDR, MTX_5_FDR, MTX_6_FDR, MTX_7_FDR, MTX_8_FDR,
					DCA_1_FDR, DCA_2_FDR, DCA_3_FDR, DCA_4_FDR, DCA_5_FDR, DCA_6_FDR, DCA_7_FDR, DCA_8_FDR,
					DCA_9_FDR, DCA_10_FDR, DCA_11_FDR, DCA_12_FDR, DCA_13_FDR, DCA_14_FDR, DCA_15_FDR, DCA_16_FDR};
//
HANDLE	thread[MNUM];	// thread handle (0 means no running thread)
float	mvalue[MNUM];	// fader value at timed mute start
int		ivalue[MNUM];	// thread index value to be thread safe
//
// program parameters
int		muteslopet;		// delay between fader update
float	mutesloped;		// dB change per fader update
float	muteslopea;		// 0/1 depending on dynamic changing of dB per fader update
//
//
int		GUI;				// by default, the Windows GUI version is selected
int		keep_running = 1;	// Main loop control
char  	wingip[24] = "";	// string for IP
int		wconnected = 0;		// 1 if connected
//
//Windows GUI handles
#ifdef __WIN32__
WINBASEAPI HWND WINAPI GetConsoleWindow(VOID);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
HINSTANCE hPrevInstance = 0;
PWSTR pCmdLine = 0;
int nCmdFile = 0;
HINSTANCE		hInstance = 0;
HWND			hdB_st, hTi_st, hAc_st, hIP_st, hSlope, hConnx;
HFONT			hfont, htmp;
HDC				hdc, hdcMem;
HBITMAP			hBmp;
BITMAP			bmp;
PAINTSTRUCT		ps;
MSG				wMsg;
#endif


int wmainloop();


#if defined(_WIN32)
//
// Windows main function and main loop
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdFile) {
//
	WNDCLASSW wc = {0};
	wc.lpszClassName = L"wtimedmutes";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	hBmp = CreateBitmap(128, 128, 1, 32, WRRIcon);

//
	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName,
		L"wtimedmutes - Manage WING mutes fade effect",
		WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU, 100, 140,
		400, 160, 0, 0, hInstance, 0);
//
//
// Main loop
	wmainloop();
	return (int) wMsg.wParam;
}
//
//
// Windows Callbacks
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
//
	char	str1[64];
	//
	switch (msg) {
	case WM_CREATE:
		//
		hIP_st = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 250, 25, 140, 20, hwnd, (HMENU)0, NULL, NULL);
		hdB_st = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 180, 45, 70, 20, hwnd, (HMENU)0, NULL, NULL);
		hTi_st = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 330, 45, 50, 20, hwnd, (HMENU)0, NULL, NULL);
		hAc_st = CreateWindow("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER, 180, 65, 70, 20, hwnd, (HMENU)0, NULL, NULL);
		//
		hConnx = CreateWindow("button", "Search WING",
				WS_VISIBLE | WS_CHILD, 135, 88, 95, 40, hwnd, (HMENU)1, NULL, NULL);
		hSlope = CreateWindow("button", "Reset",
				WS_VISIBLE | WS_CHILD, 235, 88, 70, 40, hwnd, (HMENU)2, NULL, NULL);
		hSlope = CreateWindow("button", "Set Slope",
				WS_VISIBLE | WS_CHILD, 310, 88, 80, 40, hwnd, (HMENU)3, NULL, NULL);		//
		// Display default values
		sprintf(str1, "%6.2f", mutesloped);
		SetWindowText(hdB_st, str1);
		sprintf(str1, "%d", muteslopet);
		SetWindowText(hTi_st, str1);
		sprintf(str1, "%6.2f", muteslopea);
		SetWindowText(hAc_st, str1);
		//
		if (wingip[0]) {
			SetWindowText(hIP_st, wingip);
			SetWindowText(hConnx, "Running..");
		}
		break;
	//
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		SetBkMode(hdc, TRANSPARENT);
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBmp);
		BitBlt(hdc, 1, 1, 128, 128, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);
		//
		hfont = CreateFont(16, 0, 0, 0, FW_REGULAR, 0, 0, 0, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		htmp = (HFONT) SelectObject(hdc, hfont);
		TextOut(hdc, 135, 3, str1, wsprintf(str1, "wtimedmutes - ver 3.1 - ©2021 - P-G Maillot"));
		TextOut(hdc, 135, 26, str1, wsprintf(str1, "WING IP Address:"));
		TextOut(hdc, 150, 47, str1, wsprintf(str1, "dB:"));
		TextOut(hdc, 260, 47, str1, wsprintf(str1, "Time (ms):"));
		TextOut(hdc, 135, 67, str1, wsprintf(str1, "Accel:"));
		DeleteObject(htmp);
		DeleteObject(hfont);
		//
		// update user information
		EndPaint(hwnd, &ps);
		break;
	//
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {	// user action
			int t;
			float a, d;
			switch (LOWORD(wParam)) {
			case 1:
				// Connect to IP address in edit box
				if (wconnected) {
					SetWindowText(hConnx, "Connect");
					wClose();
				}
				GetWindowText(hIP_st, wingip, GetWindowTextLength(hIP_st) + 1);
				if (wOpen(wingip) != WSUCCESS) {
					wingip[0] = 0;
					SetWindowText(hIP_st, wingip);
					MessageBox(NULL, "Invalid IP", "Warning!", MB_ICONWARNING);
					wconnected = 0;
				} else {
					SetWindowText(hIP_st, wingip);
					SetWindowText(hConnx, "Running..");
					wconnected = 1;
				}
				break;
			case 2:
				// Reset ... We cheat by accelerating :)
				// Multi-threading takes care of the rest
				a = muteslopea;
				muteslopea = 10000.;
				millisleep(300);
				muteslopea = a;
				break;
			case 3:
				// Set Slope button clicked!
				// Read new values
				GetWindowText(hdB_st, str1, GetWindowTextLength(hdB_st) + 1);
				sscanf(str1, "%f", &d);
				GetWindowText(hTi_st, str1, GetWindowTextLength(hTi_st) + 1);
				sscanf(str1, "%d", &t);
				GetWindowText(hAc_st, str1, GetWindowTextLength(hAc_st) + 1);
				sscanf(str1, "%f", &a);
				if (d * a * t < 0.0) {
					MessageBox(NULL, "Invalid Fade slope", "ATTENTION!", MB_ICONERROR);
					// Reset values
					sprintf(str1, "%6.2f", mutesloped);
					SetWindowText(hdB_st, str1);
					sprintf(str1, "%d", muteslopet);
					SetWindowText(hTi_st, str1);
					sprintf(str1, "%6.2f", muteslopea);
					SetWindowText(hAc_st, str1);
				} else {
					mutesloped = d;
					muteslopet = t;
					muteslopea = a;
				}
				break;
			}
		}
		break;
//
	case WM_DESTROY:
		// Quit
		keep_running = 0;
		PostQuitMessage(0);
		break;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}
#endif


//
//
// help file printing for command-line program
void whelp() {
	printf("Usage: wtimedmutes [-i ipv4 address]\n");
	printf("                   [-a [0], accelerated slope if != 0\n");
	printf("                   [-d [3], dB value (ex: 1.3, 3.0,...]\n");
	printf("                   [-t [20], time between changes in ms]\n");
	printf("                   [-g [0/1], start in GUI mode [1]]\n");
	printf("                   [-h] this help");
	return;
}
//
// Search for token in array
int InMuteArray(wtoken tok) {
	for (int i = 0; i < MNUM; i++) {
		if (mutes[i] == tok) return i;
	}
	return -1;
}
//
// Thread routine to manage channel strip 'i' mute
// In Win32, as soon as the thread's main function, called ThreadProc in the documentation,
// finishes, the thread is cleaned up. Any resources allocated inside the ThreadProc need
// to be cleaned up explicitly.
void *Softmute(int* i) {
	int k = *i;
	float dB = mutesloped;
	float fvalue = mvalue[k];
	while (fvalue > -87.) {
		fvalue -= dB;
		wSetTokenFloat(fader[k], fvalue);
		millisleep(muteslopet);	// wait x ms
		if (muteslopea != 0.0)	dB += mutesloped * muteslopea;
	}
	//finally, set mute
	wSetTokenInt(mutes[k], 1);
	// and return fader to its position
	wSetTokenFloat(fader[k], mvalue[k]);
	//
	thread[k] = 0;
	return 0;
}
//
int main(int argc, char **argv) {
	int		i;
	//
	// set defaults
	muteslopea = 0.;
	mutesloped = 3.0;
	muteslopet = 20;
	GUI = 1;
	//
	// any arguments passed?
	while ((i = getopt(argc, argv, "g:i:a:d:t:h")) != -1) {
		switch ((char)i) {
		case 'g':
			sscanf(optarg, "%i", &GUI);
			break;
		case 'i':
			strcpy(wingip, optarg );
			break;
		case 'a':
			sscanf(optarg, "%f", &muteslopea);
			break;
		case 'd':
			sscanf(optarg, "%f", &mutesloped);
			break;
		case 't':
			sscanf(optarg, "%d", &muteslopet);
			break;
		default:
		case 'h':
			whelp();
			return(0);
			break;
		}
	}
	//
	// we don’t know the IP of our console…
	if (wOpen(wingip) != WSUCCESS) {
		printf("WING not found at IP: %s\n", wingip);
		wconnected = 0;
//		exit(1);
	} else {
		printf("WING found at IP: %s\n", wingip);
		printf("Using wapi version %i.%i\n\n", wVer()/256, wVer()&255);
		wconnected = 1;
	}
	//
	// initialize thread array
	for (i = 0; i < MNUM; i++) {
		thread[i] = 0;
		ivalue[i] = -1;
	}
	if (GUI) {
#ifdef __WIN32__
		ShowWindow(GetConsoleWindow(), SW_HIDE); // Hide console window
		wWinMain(hInstance, hPrevInstance, pCmdLine, nCmdFile);
#else
		prinft("GUI mode not supported outside of Windows\n");
#endif
		return 0;
	} else {
		printf("wtimedmutes - ver 3.1 - (c)2021 - P-G Maillot\n");
		fflush(stdout);
	}
	//
	wmainloop();
	return 0;
}
//
//
//
int wmainloop() {
	wTV				tv[256];
	int				i, e, l;
	//
	while(keep_running) {
		if (GUI) {
			if(PeekMessage(&wMsg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&wMsg);
				DispatchMessage(&wMsg);
			}
		}
		if (wconnected) {
			// the calls to wKeepAlive() help keeping the resource cost to minimal, around 0 to 0.1% of CPU
			// vs. 33%CPU if we directly loop on wGetParsedEventsTimed().
			if (wKeepAlive() < WZERO) {		// maintain communication
				printf ("error KeepAlive\n");
				fflush(stdout);
			}
			// check events with a 500us timeout
			if ((e = wGetParsedEventsTimed(tv, 500)) > 0) {
				for (l = 0; l < e; l++) {
					// is it a mute command?
					if ((i = InMuteArray(tv[l].token)) >= 0) {
						// is mute 0 or 1
						if (tv[l].d.idata == 1) {
							// immediately unset mute
							wSetTokenInt(mutes[i], 0);
							// lower fader by 3db steps every 30ms..
							wGetTokenFloat(fader[i], &(mvalue[i]));
							//.. until we hit -87dB (the remaining dBs will be taken
							// care of by the actual mute)
							if (thread[i] == 0) {
								// save i in thread reserved storage (avoids trash values)
								ivalue[i] = i;
								if ((thread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Softmute, &ivalue[i], 0, 0)) < 0) {
									printf("Could not create thread\n");
									fflush(stdout);
									thread[i] = 0;
								}
							} else {
								printf("Timed mute in progress\n");
								fflush(stdout);
							}
						}
					}
					// Avoid memory leaks
					if (tv[l].type == S32) free(tv[l].d.sdata);
				}
			}
		} else {
			millisleep(25);
		}
	}
	return(0);
}
