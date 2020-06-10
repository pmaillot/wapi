/*
 * wtimedmutes.c
 *
 *  Created on: May 27, 2020
 *      Author: Patrick-Gilles Maillot
 *      A quick demo of timed controlled audio muting...
 */
//
#include <stdio.h>
#include <string.h>
//
#include "../wapi/wapi.h"
#include "../wapi/wext.h"
//
#ifdef __WIN32__
#include <windows.h>
#define millisleep(a)	Sleep(a)
#else
#define millisleep(a)	usleep((a)*1000)
typedef unsigned short boolean;
#endif

#define NSTEPS		20

int main() {
	wtoken			mytoken;
	int				mvalue, i;
	float			fvalue, dvalue, rvalue;
	char  			wingip[24] = "";
	//
	// we don’t know the IP of our console…
	if (wOpen(wingip)!= WSUCCESS) exit(1);
	printf("WING found at IP: %s\n", wingip);
	printf("Using version %i.%i\n", wVer()/256, wVer());

	while(1) {
		if (wKeepAlive() < WZERO) {		// maintain communication
			printf ("error KeepAlive\n");
			fflush(stdout);
		}
		// act on mute request on ch 1 with a 5ms timeout
		if (wGetVoidPTokenTimed(&mytoken, &mvalue, 5000) == WSUCCESS) {
			if (mytoken == CH_1_MUTE) {
				// mute is 0 or 1
				if (mvalue == 1) {
					// immediately unset mute
					wSetTokenInt(CH_1_MUTE, 0);
					// lower fader in 20 steps over 1 second
					wGetFloatToken(CH_1_FDR, &fvalue);
					rvalue = fvalue;
					dvalue = -144. - fvalue;
					for (i = 0; i < NSTEPS; i++) {
						fvalue += dvalue/NSTEPS;
						wSetTokenFloat(CH_1_FDR, fvalue);
						millisleep(50);	// wait 50ms
					}
					// finally set mute
					wSetTokenInt(CH_1_MUTE, 1);
					// and return fader 1 to its position
					wSetTokenFloat(CH_1_FDR, rvalue);
				}
			}
		}
	}
	return(0);
}
