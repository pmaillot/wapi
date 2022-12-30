/*
 * wevents.c
 *
 *  Created on: Jun 13, 2021
 *      Author: Patrick-Gilles Maillot
 *
 *      A small utility (command line) to capture all
 *      WING events; refresh rate is 500us
 *      Use Ctrl-C to quit
 *
 *      ver 1.0: Added variable timeout value, and a 1k events buffer to match the 10kB
 *               buffer of wapi starting v 1.19
 */
#ifdef __WIN32
#include <windows.h>
#else
#include <malloc.h>
#include <stdlib.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
//
#include "../wapi/wapi.h"
#include "../wapi/wext.h"
//
// Wait for WING data (forever)
//
int main(int argc, char **argv) {
	char  			wingip[24] = "";	// storage for WING IP address
	char*			Ntoken = 0;			// pointer to token name
	unsigned int	Otoken = 0;			// token previous value
	int				events;				// number of events (or error code)
	wTV				TV[1024];			// we should get less than 1024 events every [timeout]us
	int 			timeout = 500;		// default timeout value in us
	//
	printf("wevents -v 1.0- (c)2022 - Patrick-Gilles Maillot\n");
	printf("Using wapi ver: %i.%i\n", wVer() >> 8, wVer() & 0xFF);
	if ((events = wOpen(wingip)) != WSUCCESS) return(-1);
	printf("WING found at IP: %s\n", wingip);
	//
	while ((events = getopt(argc, argv, "t:h")) != -1) {
		switch ((char)events) {
		case 't':
			sscanf(optarg, "%d", &timeout);
			break;
		default:
		case 'h':
			printf("\nusage: wevents [-t <timeout>]\n\n");
			printf("wevents captures WING events and displays them along with their\n");
			printf("respective data. The event buffer capacity is set to 1K events \n");
			printf("every 500us by default. That can be changed using the -t <timeout>\n");
			printf("option\n");
			printf("If you get \"possible event overflow\" messages, try lowering the\n");
			printf("value of <timeout>\n");
			printf("Use <ctrl>-C to exit\n\n");
			exit(0);
			break;
		}
	}
	//
	while (1) {
		wKeepAlive();
		//
		if ((events = wGetParsedEventsTimed(TV, 1024, timeout)) > 0) {
			if (events > 960) {
				printf("possible event overflow at %d events in %dus\n", events, timeout);
				fflush (stdout);
			}
			for (int i = 0; i < events; i++) {
				if (TV[i].token != $SYSCFG_CONSOLENAME) {
					if (TV[i].token != Otoken) {			// Could call wGetName() at each
						Otoken = TV[i].token;				// iteration, but this should
						Ntoken = wGetName(TV[i].token);		// save a little resource time
					}
					printf("W-> %s data = ", Ntoken);
					if (TV[i].type == I32) printf("%i\n", TV[i].d.idata);
					else if (TV[i].type == F32) printf("%.2f\n", TV[i].d.fdata);
					else {
						if (TV[i].d.sdata) {				// should then be S32
							printf("%s\n", TV[i].d.sdata);
							fflush(stdout);
						} else {
							printf("\n");
							fflush(stdout);
						}
					}
				}
				// Avoid memory leaks
				if (TV[i].type == S32) free(TV[i].d.sdata);
			}
		} else {
			if (events != WZERO) {
				printf("Error = %i\n", events); fflush(stdout);
				return(1);
			}
		}
	}
	return 0;
}

