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

	wtoken ntoken[] = {CH_1_NAME, CH_2_NAME, CH_3_NAME, CH_4_NAME};
	wtoken mtoken[] = {CH_1_MUTE, CH_2_MUTE, CH_3_MUTE, CH_4_MUTE};
	wtoken ftoken[] = {CH_1_FDR, CH_2_FDR, CH_3_FDR, CH_4_FDR};
	char  wingip[24] = "";
	int   mute;
	float fader;
	char  name[24];

	FILE*	fd;

	// we don’t know the IP of our console…
	if (wOpen(wingip)!= WSUCCESS) exit(1);
	printf("WING found at IP: %s\n", wingip);
	printf("Using version %i.%i\n", wVer()/256, wVer());
	// open the file for reading
	if ((fd = fopen("file", "r")) != 0) {
		for (int i = 0; i< 4; i++) {
			// get data from the file
			fscanf(fd, "%23s %i %f", name, &mute, &fader);
			printf("%s %i %f\n", name, mute, fader); fflush(stdout);
			// set/send values to WING;
			// we don’t care about the returned status
			wSetTokenString(ntoken[i], name);
			wSetTokenInt(mtoken[i], mute);
			wSetTokenFloat(ftoken[i], fader);
		}
	}
	while (1) {
		union {
			int		i;
			float	f;
			char	s[64];
		} vpt;
		wtoken       tokenval;

		if (wKeepAlive() < WZERO) {
			printf("error keepalive\n");
			exit(0);
		}
		if (wGetVoidPTokenTimed(&tokenval, &vpt, 1000) == WSUCCESS) {
			printf ("name: %s, ", wGetName(tokenval));
			if (wGetType(tokenval) == F32) printf ("%6.2f\n", vpt.f);
			if (wGetType(tokenval) == I32) printf ("%i\n", vpt.i);
			if (wGetType(tokenval) == S32) printf ("%s\n", vpt.s);
			if (wGetType(tokenval) == NODE) printf ("!!node\n");
			fflush(stdout);
		}
	}
	fclose(fd);
	wClose();
	exit(0);
}
