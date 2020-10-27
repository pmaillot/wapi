/*
 * wext.h
 *
 *  Created on: Apr 25, 2020
 *      Author: Patrick-Gilles Maillot
 */

#ifndef WEXT_H_
#define WEXT_H_

extern int wKeepAlive();
extern int wOpen(char* wip);
extern int wClose();
extern int wVer();
extern int wMeterUDPPort(int wport);

extern wtype wGetType(wtoken token);
extern char* wGetName(wtoken token);
extern int wGetFloatToken(wtoken tokenval, float *fval);
extern int wGetIntToken(wtoken tokenval, float *fval);
extern int wGetStringToken(wtoken tokenval, float *fval);
extern int wGetFloatTokenTimed(wtoken tokenval, float *fval, int timeout);
extern int wGetIntTokenTimed(wtoken tokenval, int *ival, int timeout);
extern int wGetStringTokenTimed(wtoken tokenval, char* str, int timeout);
extern int wGetVoidPToken(wtoken *tokenval, void *vpt);
extern int wGetVoidPTokenTimed(wtoken *tokenval, void *vpt, int timeout);

extern int wSetTokenFloat(wtoken tokenval, float fval);
extern int wSetTokenInt(wtoken tokenval, int ival);
extern int wSetTokenString(wtoken tokenval, char* str);

extern int wSetNode(char *str);
extern int wGetNode(wtoken node, char *str);
extern int wGetNodeTimed(wtoken node, char *str, int timeout);

extern int wRenewMeters(int reqID);
extern int wSetMetersRequest(int reqID, unsigned char *wMid);
extern int wGetMeters(unsigned char *buf, int maxlen, int timeout);

#endif /* WEXT_H_ */
