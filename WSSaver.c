/*
 * WSSaver.c
 *
 *  Created on: Nov 4, 2020
 *      Author: Patrick-Gilles Maillot
 *
 *      A WING Screen Saver application, using wapi
 *
 * At init, the application gets the current values of lights and saves them as
 * 'high' lights values while setting the corresponding 'low' values their respective
 * minimum
 *
 * WING then enter screen saver mode. As soon as a surface element is modified/touched,
 * WING exits screen saver mode and the lights get 'high' values until a timeout with
 * no surface or client action taking place expires.
 *
 * The default timeout is 20s and can beset when launching the application with
 * the '-t time' option. time is provided in seconds.
 *
 * See the '-h'option for a list of options
 *
 */
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <windows.h>
//
#include "../wapi/wapi.h"
#include "../wapi/wext.h"
//
char  		wingip[24] = "";
int			keep_running = 1;
int			WSSaverTimer = 20;	// Timer value, override with -t option
//
time_t  	WingRemNow;			// 'now' time value (seconds)
time_t 		WingSsavTmr;		// Saver time value (seconds)
//
int main(int argc, char **argv) {
	int		i;
	wTV		tv;
	int		WingSSaveOn = 0;
	int		WingLoValues[11];
	int		WingHiValues[11];
	int		WingLTokens[11] = {
			$CTL_CFG_LIGHTS_BTNS,
			$CTL_CFG_LIGHTS_LEDS,
			$CTL_CFG_LIGHTS_METERS,
			$CTL_CFG_LIGHTS_RGBLEDS,
			$CTL_CFG_LIGHTS_CHLCDS,
			$CTL_CFG_LIGHTS_CHLCDCTR,
			$CTL_CFG_LIGHTS_CHEDIT,
			$CTL_CFG_LIGHTS_MAIN,
			$CTL_CFG_LIGHTS_GLOW,
			$CTL_CFG_LIGHTS_PATCH,
			$CTL_CFG_LIGHTS_LAMP
	};
	//
	printf ("WSSaver - WING Screen Saver - (c)2020 - Patrick-Gilles Maillot\n");
	fflush(stdout);
	while ((i = getopt(argc, argv, "i:t:h")) != -1) {
		switch ((char)i) {
		case 'i':
			strcpy(wingip, optarg );
			break;
		case 't':
			sscanf(optarg, "%d", &WSSaverTimer);
			break;
		default:
		case 'h':
			printf("Usage: WSSaver [-i ipv4 address]\n");
			printf("               [-t time (s)]\n");
			printf("               [-h, this help]\n");
			return(0);
			break;
		}
	}
	//
	// Establish TCP connection with the console
	if ((i = wOpen(wingip)) != WSUCCESS) {
		printf ("WSSaver: can't connect\n");
		exit(1);
	}
	//
	// Initial High and Low values
	for (i = 0; i < 11; i++ ) {
		wGetIntToken(WingLTokens[i], &WingHiValues[i]);
		WingLoValues[i] = 0;
	}
	//
	WingSsavTmr = time(NULL) + WSSaverTimer;
	while (keep_running) {
		if (wKeepAlive() < WZERO) {		// Maintain communication
			printf ("WSSaver: error KeepAlive\n");
		}
		// Get current time
		WingRemNow = time(NULL);
		// React to WING events with a 5ms timeout
		if (wGetVoidPTokenTimed(&tv, 5000) == WSUCCESS) {
			// Filter for lights related messages
			if ((tv.token < $CTL_CFG_LIGHTS_BTNS) || (tv.token > $CTL_CFG_LIGHTS_LAMP)) {
				if (WingSSaveOn) {
					// Exit screen saver mode
					for (i = 0; i < 11; i++) wSetTokenInt(WingLTokens[i], WingHiValues[i]);
					WingSSaveOn = 0;		// Screen saver OFF
					printf ("screensaver OFF\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"); fflush(stdout);
				}
				WingSsavTmr = WingRemNow + WSSaverTimer;
			} else {
				// Get new low lights values
				if (WingSSaveOn) WingLoValues[tv.token - $CTL_CFG_LIGHTS_BTNS] = tv.d.idata;
				// Get new high lights values
				else WingHiValues[tv.token - $CTL_CFG_LIGHTS_BTNS] = tv.d.idata;
			}
		} else {
			// No WING data
			if (!WingSSaveOn) {
				// If not yet set, test timing condition to set screen saver state
				if (WingRemNow > WingSsavTmr) {
					for (i = 0; i < 11; i++)  wSetTokenInt(WingLTokens[i], WingLoValues[i]);
					WingSSaveOn = 1;	// Screen saver ON
					printf ("screensaver ON \b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"); fflush(stdout);
				}
			}
		}
		// sleep 10ms - low cpu usage
		Sleep(10);
	}
	return 0;
}

