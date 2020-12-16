/*
 * wtest.c
 *
 *  Created on: Apr 18, 2020
 *      Author: Patrick-Gilles Maillot
 */
#define _WIN32_WINNT  0x501
//
#include <stdio.h>
#include <string.h>
#include <time.h>
//
#include "wapi.h"
#include "wext.h"
//
#ifdef __WIN32__
#include <windows.h>
#define millisleep(a)	Sleep(a)
#else
#define millisleep(a)	usleep((a)*1000)
typedef unsigned short boolean;
#endif


int main() {
	int		i;
	char  	wingip[24] = "";
	wTV		TV;

	if ((i = wOpen(wingip)) != WSUCCESS) return(-1);
	printf("WING found at IP: %s\n", wingip);
	printf("Using version %i.%i\n", wVer()/256, wVer());

	while (1) {
		wKeepAlive();
		//
		if ((i = wGetVoidPTokenTimed(&TV, 1000)) == WSUCCESS) {
			printf("W-> %s type = %i, data = ", wGetName(TV.token), TV.type);
			if (TV.type == I32) printf("%i\n", TV.d.idata);
			if (TV.type == F32) printf("%.2f\n", TV.d.fdata);
			if (TV.type == S32) {
				printf("%s\n", TV.d.sdata);
				if (TV.d.sdata) free(TV.d.sdata);
			}
			fflush(stdout);
		} else {
			if (i != WZERO) {
				printf("Error = %i\n", i); fflush(stdout);
			}
		}
	}
	return 0;
}

