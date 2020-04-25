# wapi
A C language API for WING digital mixers

This repo is a C code API to WING digital mixing consoles. Ver 0.1 (first release) is capable of get and set *all* WING parameters according to FW 1.06.
This represents more than 24000 parameters!

The API uses UDP and TCP networking to establish a connection to the WING console. Typical communication take place over TCP/IP on port 2222.

Parameters can be nodes, int, float or string

Get functions can be attended, timed, or unattended (such as in even loops)
Set functions are... set :)

Node management will be part of a later release.

# use
At the API level, WING data can be 32bit int, 32bit float or string data. All API data in little-endian, enabling easy use in standard programming languages.

Besides this document, the wapi API consists of two include files and a library:
wapi.h is the main include file containing enumerated types for errors, token types, and wapi abstraction enumerated tokens.
wext.h is a file containing the definitions of external library calls to wapi.lib the actual library of API functions.
wapi.lib is s static-link library for linking with your application. The library contains all wapi functions currently available.

A typical program accessing WING starts with an ‘open’ function and ends with a ‘close’ function. These two functions establish the communication path to WING on your local network and ensure data is properly cleaned when leaving the program.

Programs communicate with WING over network. The API call wOpen() is used to establish communication link between WING and the application. 

WING supports multiple formats, including integers, floats, and strings types. The API will try to ensure conversions as best as possible in order to match the requested format either by WING or by the API command. For example, if you request float data from a WING token which is an integer, the API will convert the integer to float before returning the data. Similarly, if you set a WING token of type string by sending it a float value, the float data will be changed to string before being sent to WING.

All WING tokens are identified by their name, based on the JSON structure that represents WING’s hierarchical data tree we already presented in this document.
For example the identifier for channel 1 equalizer, setting 2 frequency control, a.k.a “ch.1.eq.2f” in the JSON tree is known as token CH_1_EQ_2. The respective data in WING is a float and as written above, it can be modified from an integer, a float or even a string value, and can be returned to the application as an integer, a float of even a string.

Some tokens are read-only; trying to change their value will result in an error returned to the application.

WING tokens are listed in an include file: wapi.h that must be included in your program. The include file also contains the status or error codes that can be returned by the API function calls.

All function calls are regrouped in a binary library: wapi.lib, that you must include at link time. 
A typical compilation of a source file wtest.c in a Windows environment can be as follows:
gcc -O3 -Wall -c -fmessage-length=0 -o wtest.o "wtest.c" 
gcc "-LC:<path to wapi.lib>" -o wtest.exe wtest.o -lwapi -lws2_32

Don’t forget to set the correct path to the wapi.lib file in the above compilation/link likes!


WING parameters can be set (or modified) using the wSetxxx API family of calls; Similarly, the parameters can be retrieved from WING using the wGetxxx API family of calls. 


# Wapi Reference Guide

# Open and Close

# int wOpen(char* wip)
wOpen() initializes global variables for the application and opens the communication with a WING console responding at IP address wip.
wip is a string containing the console IP data in the form “xxx.xxx.xxx.xxx”; if the console IP address is unknown, wip should be an empty string and provide enough characters to store the IP address where WING will be found. The wOpen() function will attempt a network broadcast announce to identify the first WING that will reply on the local network.
Upon successful completion the function will return WSUCCESS and if the wip parameter was an empty string when calling the function, it will contain the IP at which the console was found. Other values can be returned in case of issues or errors reported.
Once connection is established with the console, it will be kept active for about 10 seconds after which the console will close the link. The wKeepAlive() function can be called to extend the link active another 10 seconds.
It must be noted that as long as a connection is kept active, changes made directly at the console (by moving a fader, or pressing buttons for example) will generate data the application will continuously receive. This can represent a lot of data the application must be ready to accept and manage. It can also be the source of incorrect data returned to Get functions and specific care should be taken when developing live or event-driven applications.


# void wClose()
wClose() ensures data is correctly disposed of when your program ends. It should be the last call before the return statement or exit call in your application.


# int wVer()
wVer() returns the version of the wapi library file being used. The returned version is in the form ‘major.minor’, and its value is provided as 0x0000MMmm, with MM the major part of the version number and mm representing the minor part.
 
# Setting Values

# int wSetTokenFloat(wtoken token, float fval)
The wSetTokenFloat()  function takes as input a WING token token and a 32bit float value. It sends to WING the value of fval after it has been adapted to the format expected by the WING token it is sent to. 
For example, sending value 444.0 to WING token CH_1_EQ_1F will be sent as a 32bit float value. WING will nevertheless adjust it to the nearest valid value of 444.533997. Sending that same value 444.0 to WING token CH_1_EQ_ON will result in a setting to 1; Finally, sending value 444.0 to WING token CH_1_NAME will change the channel name to 444.00.
The function returns WSUCCESS if the requested operation was successful, other values can be returned, such as WZERO if no suitable format was found for adapting the value of fval, or WSEND_TCP_ERROR if an error took place while communicating with WING. Attempting to set a value on a token of type NODE will return WNODE.


# int wSetTokenInt(wtoken token, int ival)
The wSetTokenInt()  function takes as input a WING token token and a 32bit integer value. It sends to WING the value of ival after it has been adapted to the format expected by the WING token it is sent to. 
For example, sending value 444 to WING token CH_1_EQ_1F will be sent as a 32bit float value. WING will also adjust it to the nearest valid value of 444.533997. Sending that same value 444 to WING token CH_1_EQ_ON will result in a setting to 1; Finally, sending integer value 444 to WING token CH_1_NAME will change the channel name to 444.
The function returns WSUCCESS if the requested operation was successful, other values can be returned, such as WZERO if no suitable format was found for adapting the value of ival, or WSEND_TCP_ERROR if an error took place while communicating with WING. Attempting to set a value on a token of type NODE will return WNODE.



# int wSetTokenString(wtoken token, char* str)
The wSetTokenString()  function takes as input a WING token token and a string str. It sends to WING the value of str after it has been adapted to the format expected by the WING token it is sent to. 
For example, sending string “444” to WING token CH_1_EQ_1F will be sent as a 32bit float value of 444.0; WING will the adjust it to the nearest valid value of 444.533997. Sending that same string “444” to WING token CH_1_EQ_ON will result in a setting to 1, Finally, sending string “444” to WING token CH_1_NAME will change the channel name to 444.
The function returns WSUCCESS if the requested operation was successful, other values can be returned, such as WZERO if no suitable format was found for adapting the string str, or WSEND_TCP_ERROR if an error took place while communicating with WING. Attempting to set a value on a token of type NODE will return WNODE.
Getting Values

# int wGetTokenFloat(wtoken token, float* fval)
The wGetTokenFloat()  function interrogates WING token token to get its currently associated value. This value has a given type, which can for example be byte, 16bits integer, 32bits integer or float, empty string, 64 or 256 chars max string. The value retrieved from WING is adapted to float format as expected by the fval variable.
For example, inquiring WING token CH_1_EQ_1F will return the current value of the token as a float value in fval. Inquiring WING token CH_1_EQ_ON will result in a value of 0.0 or 1.0, depending on the state of the token. 
On the other hand, inquiring WING token CH_1_NAME will return a value of 0.0 and a status of WZERO.
The function returns WSUCCESS if the requested operation was successful, other values can be returned, such as WZERO if no suitable format was found for adapting token value to fval, or WSEND_TCP_ERROR if an error took place while communicating with WING. Attempting to get a value from a token of type NODE will return WNODE.



# int wGetTokenInt(wtoken token, int* ival)
The wGetTokenFloat()  function interrogates WING token token to get its currently associated value. This value has a given type, which can for example be byte, 16bits integer, 32bits integer or float, empty string, 64 or 256 chars max string. The value retrieved from WING is adapted to integer format as expected by the ival variable.
For example, inquiring WING token CH_1_EQ_1F will return the current value of the token as an int value in ival. Inquiring WING token CH_1_EQ_ON will result in a value of 0 or 1, depending on the state of the token. 
On the other hand, inquiring WING token CH_1_NAME will return a value of 0.0 and a status of WZERO.
The function returns WSUCCESS if the requested operation was successful, other values can be returned, such as WZERO if no suitable format was found for adapting token value to ival, or WSEND_TCP_ERROR if an error took place while communicating with WING. Attempting to get a value from a token of type NODE will return WNODE.


# int wGetTokenString(wtoken token, char* str)
The wGetTokenFloat()  function interrogates WING token token to get its currently associated value. This value has a given type, which can for example be byte, 16bits integer, 32bits integer or float, empty string, 64 or 256 chars max string. The value retrieved from WING is adapted to string format as expected by the str variable.
For example, inquiring WING token CH_1_EQ_1F will return the current value of the token as a string in str. Inquiring WING token CH_1_EQ_ON will result in a 1-character string of “0” or “1”, depending on the state of the token. Similarly, a token with a floating-point native format would result in a string containing the string representation of the floating-point value.
As a last example, inquiring WING token CH_1_NAME will return the string currently used for naming channel 1.
The function returns WSUCCESS if the requested operation was successful, other values can be returned, such as WZERO if no suitable format was found for adapting token value to str, or WSEND_TCP_ERROR if an error took place while communicating with WING. Attempting to get a value from a token of type NODE will return WNODE.

The Get functions presented above are all “one shot read” functions so to speak; They request data from WING, expect the right data to be returned and upon a single TCP operation retrieving the currently available network record, they will return the buffer content, adapting it to the requested type of data. This is the most efficient way to gather information from the console, but come with a major caveat if someone is also manipulating (locally or remotely) the desk. Indeed, as other changes take place and assuming your communication channel is in an ‘open’ state (i.e. your last communication with WING is less than 10s old), the console will natively send you changes that are taking place, resulting of the local or remote changes operated onto the desk. 
So, when a “one shot read” request arrives and is served, it is very likely to get data that does not correspond to the token that was part of the Get function, resulting in inconsistent data.

Wapi therefore provides another set of Get functions for applications requiring a tighter control over the data they exchange with WING. In this new set of functions, the Get instance will gather information from WING over a given period of time and filter the possibly multiple  received tokens for one matching the specified token provided at call time. Only when the specified token is received or time has expired will the function process the data it received.


# int wGetFloatTokenTimed(wtoken tokenval, float *fval, int timeout)
The int wGetFloatTokenTimed() function is to some extend similar to the wGetFloatToken() function in the sense it aims at retrieving from WING data and adapt it to floating-point format before returning it to fval;
But it will do so over a period of time timeout, expressed in µs (microseconds). 
As long as timeout is not reached, the function is inquiring WING for data, if no data appears available, a value of WZERO is returned. 
If on the contrary data is available from WING, the function will check if the data token is the correct one; it will treat the data as done in the wGetFloatToken() function; i.e. the value retrieved from WING is adapted to float format as expected by the fval variable.
For example, inquiring WING token CH_1_EQ_1F will return the current value of the token as a float value in fval. Inquiring WING token CH_1_EQ_ON will result in a value of 0.0 or 1.0, depending on the state of the token. 
On the other hand, inquiring WING token CH_1_NAME will return a value of 0.0 and a status of WZERO.
The function returns WSUCCESS if the requested operation was successful, other values can be returned, such as WZERO if no suitable format was found for adapting token value to fval, or WSEND_TCP_ERROR if an error took place while communicating with WING. Attempting to get a value from a token of type NODE will return WNODE.
If data is available from WING and the data token is not the expected one, the function discards data and inquires WING for new data. The above takes place as long as timeout is not reached.



# int wGetIntTokenTimed(wtoken tokenval, int *ival, int timeout)
The int wGetIntTokenTimed() function is to some extend similar to the wGetIntToken() function in the sense it aims at retrieving from WING data and adapt it to floating-point format before returning it to ival;
But it will do so over a period of time timeout, expressed in µs (microseconds). 
As long as timeout is not reached, the function is inquiring WING for data, if no data appears available, a value of WZERO is returned. 
If on the contrary data is available from WING, the function will check if the data token is the correct one; it will treat the data as done in the wGetIntToken() function; i.e. the value retrieved from WING is adapted to float format as expected by the ival variable.
For example, inquiring WING token CH_1_EQ_1F will return the current value of the token as an int value in ival. Inquiring WING token CH_1_EQ_ON will result in a value of 0 or 1, depending on the state of the token. 
On the other hand, inquiring WING token CH_1_NAME will return a value of 0.0 and a status of WZERO.
The function returns WSUCCESS if the requested operation was successful, other values can be returned, such as WZERO if no suitable format was found for converting the retrieved value to ival, or WSEND_TCP_ERROR if an error took place while communicating with WING. Attempting to get a value from a token of type NODE will return WNODE.
If data is available from WING and the data token is not the expected one, the function discards data and inquires WING for new data. The above takes place as long as timeout is not reached.



# int wGetStringTokenTimed(wtoken tokenval, char* str, int timeout)
The int wGetStringTokenTimed() function is to some extend similar to the wGetStringToken() function in the sense it aims at retrieving from WING data and format it as a string;
But it will do so over a period of time timeout, expressed in µs (microseconds). 
As long as timeout is not reached, the function is inquiring WING for data, if no data appears available, a value of WZERO is returned. 
If on the contrary data is available from WING, the function will check if the data token is the correct one;
it will then treat the data as done in the wGetStringToken() function; The value retrieved from WING is adapted to string format as expected by the str variable. Similar restrictions and conversion rules apply. Therefore and as examples, inquiring WING token CH_1_EQ_1F will return the current value of the token as a string in str. Inquiring WING token CH_1_EQ_ON will result in a 1-character string of “0” or “1”, depending on the state of the token. Similarly, a token with a floating-point native format would result in a string containing the string representation of the floating-point value. As a last example, inquiring WING token CH_1_NAME will return the string currently used for naming channel 1. Attempting to get a value from a token of type NODE will return WNODE.
If data is available from WING and the data token is not the expected one, the function discards data and inquires WING for new data. The above takes place as long as timeout is not reached.


 
# A Small Program Example
Let’s program! Assume you need to programmatically change the name of channels and mute/unmute the respective channels from data contained in a file. Let’s consider the file also contains initial channel faders, and covers channels 1 to 4. The file can be a text file such as:



    Steve  0  -144.0
    Jimmy  1  -30.0
    Carla  1  -22.0
    Jannet 0  -100.0



This is C source code; easy to understand and translate if needed to other programming languages.
We show on the right of the page the resulting channel strips 1-4:



    #include <stdio.h>
    #include <string.h>
    //
    #include "wapi.h"
    #include "wext.h"
    //
    int main() {
        wtoken ntoken[] = {CH_1_NAME, CH_2_NAME, CH_3_NAME, CH_4_NAME};
        wtoken mtoken[] = {CH_1_MUTE, CH_2_MUTE, CH_3_MUTE, CH_4_MUTE};
        wtoken ftoken[] = {CH_1_FDR, CH_2_FDR, CH_3_FDR, CH_4_FDR};
        char   wingip[24] = "";
        int    mute;
        float  fader;
        char   name[24];
        FILE*  fd;
        //
        // we don’t know the IP of our console…
        if (wOpen(wingip)!= WSUCCESS) exit(1);
        printf("WING found at IP: %s\n", wingip);
        // open the file for reading
        if ((fd = fopen("file", "r")) != 0) {
            for (int i = 0; i< 4; i++) {
                // get data from the file
                fscanf(fd, "%23s %i %f", name, &mute, &fader);
                printf("%s %i %f\n", name, mute, fader);
                // set/send values to WING;
                // we don’t care about the returned status
                wSetTokenString(ntoken[i], name);
                wSetTokenInt(mtoken[i], mute);
                wSetTokenFloat(ftoken[i], fader);
            }
        }
        fclose(fd);
        wClose();
        exit(0);
    }	 


 
# Unsolicited updates
There are times and situations when WING will send data to your program. This has been explained above: As soon as you are connected to WING and have exchanged data with it, the connection will stay in an open state for 10s, unless to specifically establish and close the TCP connection around your work. While this will help, it will not prevent WING to send you data while the TCP link is active, and is certainly not an effective way to manage data as you will send more resources in opening/closing the connection than in time sending or receiving data.
Wapi provides additional API functions to manage event driven applications. These are managed around the notion of ‘main loop’ as often found in IOT devices running Arduino devices, or in standard Linux or Windows applications where a main loop ensures the management of all events coming from the devices connected to your computer (mouse, keyboard, etc.). WING data can be treated just as any other event.
API calls are therefore available to keep a connection between your application and WING alive, as well as to get data from WING, effectively emptying the event queue of the communication with the console. This will be assured with the wKeepAlive() and the wGetVoidPTokenTimed() function calls presented below.

# int wKeepAlive()
wKeepAlive() maintains the connection between WING and the calling program so data issued by the console with no request initiated by the program can be received in a main loop, or over an extended period of time beyond 10s. 
In fact, this function can be called as often as you like and will optionally performs a small exchange with the console, based on an internal timer. The elapsed time between two effective exchanges of data with the console depend on the value of wKeepAlive_TIMEOUT which is part of the wapi.h file.
The wKeepAlive() function returns WSUCCESS if a valid exchange took place to renew a 10 seconds working communication,  or WZERO if no exchange was necessary. The function can also return the values of WSEND_ERROR or WRECV_ERROR if communication was not successful.


# int wGetVoidPTokenTimed(wtoken *token, void* vpt, int timeout)()
The wGetVoidPTokenTimed() API call is a specific Get function. Unlike other Get functions previously presented in this document, it does not expect data from a specific token, nor a specified format in which the data from the console should be converted to. The function will check the WING receive event queue for event and remove the oldest event available from the queue. This check will last for a time specified by timeout, expressed in micro seconds. If no data is found over a period of time of value timeout, a value of WZERO is returned. 
When data is available in the event queue, the oldest even is retrieved, and its token is returned in the token variable. The data associated to the token is also returned to the calling application using the vpt variable. The function returns WSUCCESS if data has been returned to the calling program, WZERO if no suitable format conversion was found or a timeout occurred. It can also return WRECV_ERROR on TCP read errors. Attempting to get a value from a token of type NODE will return WNODE.

In a typical, simple example of use of the two API calls shown in the following paragraph, the main loop is replaced with a while(1){} statement.



    

    #include "wapi.h"
    #include "wext.h"

    
        while (1) {
            union {
                int       i;
                float     f;
                char	     s[64];
            } vpt;
            Wtoken        tokenval;

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
    




 


