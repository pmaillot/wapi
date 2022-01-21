/*
 * wext.h
 *
 *  Created on: Apr 25, 2020
 *      Author: Patrick-Gilles Maillot
 */
#ifndef WEXT_H_
#define WEXT_H_
//
extern int wKeepAlive();
extern int wOpen(char* wip);
extern int wClose();
extern int wVer();
extern int wMeterUDPPort(int wport);
//
extern wtype wGetType(wtoken token);
extern char* wGetName(wtoken token);
//
extern int wGetTokenDef(wtoken token, int *num, unsigned char* *str);
//
extern int wGetToken(wtoken tokenval, wtype *type, wvalue *value);
extern int wGetTokenFloat(wtoken tokenval, float *fval);
extern int wGetTokenInt(wtoken tokenval, int *ival);
extern int wGetTokenString(wtoken tokenval, char* str);
extern int wGetParsedEvents(wTV *tv);
//
extern int wGetTokenTimed(wtoken tokenval, wtype *type, wvalue *value, int timeout);
extern int wGetTokenFloatTimed(wtoken tokenval, float *fval, int timeout);
extern int wGetTokenIntTimed(wtoken tokenval, int *ival, int timeout);
extern int wGetTokenStringTimed(wtoken tokenval, char* str, int timeout);
extern int wGetParsedEventsTimed(wTV *tv, int timeout);
//
extern int wSetTokenFloat(wtoken tokenval, float fval);
extern int wSetTokenInt(wtoken tokenval, int ival);
extern int wSetTokenString(wtoken tokenval, char* str);

//
extern int wSetNode(char *str);
extern int wSetNodeFromTVArray(wTV *array, int nTV);
extern int wGetNode(wtoken node, char *str);
extern int wGetNodeToTVArray(wtoken node, wTV *array);
extern int wGetBinaryNode(wtoken node, unsigned char *rbuf, int maxlen);
extern int wSetBinaryNode(unsigned char *rbuf, int len);
//
extern int wRenewMeters(int reqID);
extern int wSetMetersRequest(int reqID, unsigned char *wMid);
extern int wGetMeters(unsigned char *buf, int maxlen, int timeout);
//
#endif /* WEXT_H_ */
