/*
 * wevents.c
 *
 *  Created on: Jun 13, 2021
 *      Author: Patrick-Gilles Maillot
 *
 *      A small utility (command line) to capture all
 *      WING events; refresh rate is 500us
 *      Use Ctrl-C to quit
 */
#include <windows.h>
#include <stdio.h>
#include <string.h>
//
#include "../wapi/wapi.h"
#include "../wapi/wext.h"
//
// Wait for WING data (forever)
//
int main() {
	char  			wingip[24] = "";	// storage for WING IP address
	char*			Ntoken = 0;			// pointer to token name
	unsigned int	Otoken = 0;			// token previous value
	int				events;				// number of events (or error code)
	wTV				TV[128];			// we should get less than 128 events every 100us

	if ((events = wOpen(wingip)) != WSUCCESS) return(-1);
	printf("WING found at IP: %s\n", wingip);
	printf("Using wapi ver: %i.%i\n", wVer() >> 8, wVer() & 0xFF);

	while (1) {
		wKeepAlive();
		//
		if ((events = wGetParsedEventsTimed(TV, 500)) > 0) {
			for (int i = 0; i < events; i++) {
				if (TV[i].token != Otoken) {			// Could call wGetName() at each
					Otoken = TV[i].token;				// iteration, but this should
					Ntoken = wGetName(TV[i].token);		// save a little resource time
				}
				printf("W-> %s data = ", Ntoken);
				if (TV[i].type == I32) printf("%i\n", TV[i].d.idata);
				else if (TV[i].type == F32) printf("%.2f\n", TV[i].d.fdata);
				else if (TV[i].d.sdata) {				// should then be S32
					printf("%s\n", TV[i].d.sdata);
					free(TV[i].d.sdata);				// No memory leak
				}
				fflush(stdout);
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

