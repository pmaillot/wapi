/*
 * wdef.h
 *
 *  Created on: Apr 25, 2020
 *      Author: Patrick-Gilles Maillot
 *
 */

#ifndef WDEF_H_
#define WDEF_H_

#define W_MTU		1400			// MTU = 1400 for receiving data
#define wing_dport	2222			// WING discovery port
#define wing_oport	2223			// WING OSC port
#define SSIZE		64				// 64 bytes buffer size for typical set calls
#define RSIZE		2*1024			// 2kB receive buffer size (typical MSS is 1400 bytes) for typical calls
#define GET_COUNT 	50				// number of attempts at reading data before giving up on not receiving expected set
//
#define NRP_ESCAPE_CODE         0xdf
#define NRP_CHANNEL_ID_BASE     0xd0
#define NRP_NUM_CHANNELS        14

#define endian2buf(wbuf, endian, wlen)	\
	do {								\
		wbuf[wlen++] = endian.cc[3];	\
		wbuf[wlen++] = endian.cc[2];	\
		wbuf[wlen++] = endian.cc[1];	\
		wbuf[wlen++] = endian.cc[0];	\
	} while (0);


typedef unsigned int whash;

typedef struct WCommand {
	char*			command;	// - WING command name
	whash			h;			// - wingmap hash
	wtype			type;		// - value type [B32, I32, F32, S32,...] (see wtype enum)
	unsigned short	flags;		// - flags associated to the command (R/O, etc...)
	union {						// - The value associated to the format type above
		int   ii;				// can be int, float, string address or general data address
		float ff;				//
		char  cc[4];			// useful for endian conversions
		char* str;				// str will be in the form <length><string>
		void* dta;				//
	} value;
} WCommand;


typedef struct RCommand {
	whash			h;			// - wingmap hash
	wtoken			t;			// - WING token
} RCommand;



#endif /* WDEF_H_ */
