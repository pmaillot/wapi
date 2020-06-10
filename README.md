# wapi
version 0.5

A C language API for WING Digital Mixing Consoles


![WING personal digital mixing console](https://sites.google.com/site/patrickmaillot/home/wing/Wing%20Default.snap.png?attredirects=0)

This repo is a C code API to WING digital mixing consoles. Ver 0.1 (first release) is capable of get and set *all* WING parameters according to FW 1.06.
This represents more than 28000 parameters!

The API uses UDP and TCP networking to establish a connection to the WING console. Typical communication take place over TCP/IP on port 2222.

Parameters can be nodes, int, float or string

Get functions can be attended, timed, or unattended (such as in event programming/loops)
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

Don’t forget to set the correct path to the wapi.lib file in the above compilation/link lines!
Someone mentioned that the -mwindows option might be needed in the link line above, after -lws2_32; (to be validated).


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


# Getting Values

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


The Get functions presented above are all “one shot read” functions so to speak; They request data from WING, expect the right data to be returned and upon a single TCP operation retrieving the currently available network record, they will return the buffer content, adapting it to the requested type of data. This is the most efficient way to gather information from the console, but comes with a major caveat if someone is also manipulating (locally or remotely) the desk. Indeed, as other changes take place and assuming your communication channel is in an ‘open’ state (i.e. your last communication with WING is less than 10s old), the console will natively send you changes that are taking place, resulting of the local or remote changes operated onto the desk. 
So, when a “one shot read” request arrives and is served, it will sort through the received data for the expected token, and in doing this will discard the data received prior to finding the correct token.

Wapi therefore provides another set of Get functions for applications requiring a timed control over the data they exchange with WING. In this new set of functions, the Get instance will as for the non-timed versions gather information from WING over a given period of time and filter the possibly multiple  received tokens for one matching the specified token provided at call time. Only when the specified token is received or time has expired will the function process the data it received.


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



# int wGetNode(wtoken node, char *str) 
The wGetNode() function will return in str a string of values separated with spaces and corresponding to the node token node. The function returns a status WSUCCESS if the node was processed with no errors; It will return WZERO if the token provided is not a valid node. The function can also report other errors if communication issues were detected.



# int wGetNodeTimed(wtoken node, char *str, int timeout) 
The wGetNodeTimed() function will return in str a string of values separated with spaces and corresponding to the node token node. The function returns a status WSUCCESS if the node was processed with no errors; It will return WZERO if the token provided is not a valid node. The function can also report other errors if communication issues were detected. Eact node recovery attempt will be procedded over a time of value timeout; passed this delay, the function will stop and return WZERO. Timeout applies to any and every of the parameters part of the node node.



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


# int wGetVoidPToken(wtoken *token, void* vpt)()
The wGetVoidPToken() API call is a specific Get function. Unlike other Get functions previously presented in this document, it does not expect data from a specific token, nor a specified format in which the data from the console should be converted to. The function will check the WING receive event queue for data and will only return when data is received by removing the oldest event available from the queue. If no valid data is found, a value of WZERO is returned. 
When data is available in the event queue, the oldest even is retrieved, and its token is returned in the token variable. The data associated to the token is also returned to the calling application using the vpt variable. The function returns WSUCCESS if data has been returned to the calling program, WZERO if no suitable format conversion was found. It can also return WRECV_ERROR on TCP read errors. Attempting to get a value from a token of type NODE will return WNODE.


# int wGetVoidPTokenTimed(wtoken *token, void* vpt, int timeout)()
The wGetVoidPTokenTimed() API call is a specific Get function. Unlike other Get functions previously presented in this document, it does not expect data from a specific token, nor a specified format in which the data from the console should be converted to. The function will check the WING receive event queue for event and remove the oldest event available from the queue. This check will last for a time specified by timeout, expressed in micro seconds. If no data is found over a period of time of value timeout, a value of WZERO is returned. 
When data is available in the event queue, the oldest even is retrieved, and its token is returned in the token variable. The data associated to the token is also returned to the calling application using the vpt variable. The function returns WSUCCESS if data has been returned to the calling program, WZERO if no suitable format conversion was found or a timeout occurred. It can also return WRECV_ERROR on TCP read errors. Attempting to get a value from a token of type NODE will return WNODE.

In a typical, simple example of use of the two API calls shown in the following paragraph, the main loop is replaced with a while(1){} statement.



    

    #include "wapi.h"
    #include "wext.h"

        ...
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
        ...



# Nodes
In many applications as well as in browsing over the JSON data structure, one can easily envision it would be interesting for optimization purposes to get and set a group of attributes at once, rather than establishing communication requests for each single parameter.

Nodes were introduced in the X32 family to enable this functionality, and have been widely used in several applications for controlling the desk; In the case of WING this may be even more interesting due to the very large number/volume of parameter data  available as one unrolls each branch in the JSON tree opening a new level of nodes and parameters. Each branch of the JSON tree can be pictured as a collection of flat layers, hence introducing the notion of nodes;
Flattening the WING JSON structure results in layers/lines of identifiers which can be nodes or parameter values. When a given layer only contains nodes, it can be removed from the list of WING nodes as no setting changes can result in consulting these. Similarly, when within a given layer, a mix of values and nodes are present, the layer can be limited to only include values, reducing the amount of data to be transferred during network communications. 

As the set of values in a node list is fixed and of known type; it can be set and retrieved as a single line of text with pre-formatted data so this is easier to store and manage in applications. Each returned string is formatted according to the Linux string definition: a series of 1byte characters, followed by a line feed character (\n) and NULL terminated (\0).  
The presence of the line feed character is only there to help creating text files directly from node data, without any need for further formatting at the application level.
The following functions list API entries to use WING nodes as defined above.


 
# int wSetNode(wtoken node, char *str

The wGetNode() function will parse the string values contained in str according to the format conversions needed to fill all values of the node provided by token node. Each value of the node will be set accordingly to the result of the conversion. 
If the list of values contained in str leads to a shorter list of attributes, the remaining attributes of the node will remain unmodified. 
The function returns a status WSUCCESS if the node was processed with no errors; It will return WZERO if the token provided is not a valid node. The function can also report other errors if communication issues were detected.



# int wGetNode(wtoken node, char *str)

The wGetNode() function will return in str a string of values separated with spaces and corresponding to the node token node. The function returns a status WSUCCESS if the node was processed with no errors; It will return WZERO if the token provided is not a valid node. The function can also report other errors if communication issues were detected. The line of text returned by the function end with a line-feed and a null byte.



# int wGetNodeTimed(wtoken node, char *str, int timeout)

As for the non-timed version, The wGetNodeTimed() function will return in str a string of values separated with spaces and corresponding to the node token node. The function returns a status WSUCCESS if the node was processed with no errors; It will return WZERO if the token provided is not a valid node. For each leaf tag part of the node being returned, the function applies a timeout timeout for retrieving data. If data cannot be gotten in time, a value of WZERO is returned. As data is retrieved from the console, the function ensures it corresponds to each respective leaf token, as unsolicited data could be presented by the console during the working cycle of the timed function.
The function can also report other errors if communication issues were detected. The line of text returned in str by the function end with a line-feed and a null byte.
Note: a value of 10000 for timeout seems a safe bet; Indeed, lower values may return from the internal function calls too soon for valid data to be returned by the function.

 
Requesting the full set of nodes from a freshly initialized console  results in a file of 3300+ lines, and more than 170kbytes. It takes approximatively 20 seconds over WIFI to execute a full dump. We show below a few lines of nodes as they are proposed by wapi when using wGetNode() or wGetNodeTimed() for channel 1:

    node ch: 
    node ch.1: 
    node ch.1.in: 
    node ch.1.in.set: M 0 0 0 0.000000 0.000000 0.000000 0 0.000000 
    node ch.1.in.conn: LCL 1 OFF 1 
    node ch.1.flt: 0 100.237457 0 10018.260742 0 TILT 0.000000 
    node ch.1.peq: 0.000000  0.000000 99.685432 1.995882 0.000000 999.250488 1.995882 0.000000 10016.527344 10016.527344  
    node ch.1.gate: 0 GATE -40.000000 40.000000 10.000000 10.000000 199.404282 0.000000 1:3 
    node ch.1.gatesc: OFF 1002.374390 1.995882 SELF IN 0 
    node ch.1.eq: 0  STD 100.000000 0 1 0.000000 80.196419 1.995882 SHV 0.000000 200.000000 1.995882 0.000000 601.388367 1.995882 0.000000 1499.788330 1.995882 0.000000 3990.524414 1.995882 0.000000 11994.417969 1.995882 SHV 
    node ch.1.dyn: 0 COMP 100.000000 0.000000 -10.000000 3.000000 3 RMS 50.000000 20.000000 152.565186 LOG 1 
    node ch.1.dynxo: 6.000000 OFF 1002.374390 0 
    node ch.1.dynsc: OFF 1002.374390 1.995882 SELF IN 0 
    node ch.1.preins: 0 NONE - 
    node ch.1.main: 
    node ch.1.main.1: 1 0.000000 



# Effects and Plugins
WING comes with an impressive number of effects, plugins and emulations that can be used on any channel without costing any FX slots. In every channel, Gate, EQ Compressor can take different processing models you can organize and change on the fly. The following pages below present the different effects and their parameters.

# Plugins
Plugins entries are directly included with channels, busses, etc. and can either default to WING standard algorithms or adapt to alternative plugins to color your sound or fit your taste when it comes to mixing. Plugins are showing under the main JSON structure, only when instantiated. WING Channel audio engines enable 4 sorts of plugins: Filter, Gate, EQ and Dynamics. Bus, Main and Matrix audio engines support EQ and Dynamics plugins.

The choice of plugin is represented by the name (or model) of the plugin, as set under the respective “mdl” token; After a console reset, the default channel Filter, Gate, EQ and Dynamics plugins will be  “TILT”, “GATE”, “STD”,  and “COMP”, respectively, and these can be changed to one of the multiple plugins available within the console (respecting the category they apply to of course).
The choice of plugin is represented by the name (or model) of the plugin, as set under the respective “mdl” token; authorized values are:

# Filters:
    TILT EQ, MAXER, AP 90, AP 180

# Gates:
    GATE/EXPANDER, DUCKER, EVEN 88 GATE, SOUL 9000 GATE, DRAW MORE 241, BDX902 DEESSER, WAVE DESIGNER, DYNAMIC EQ, SOUL WARMTH PRE, 76 LIMITER AMP, LA LEVELER, AUTO RIDER

# Equalizers:
    WING EQ, SOUL ANALOGUE, EVEN 88 FORMANT, EVEN 84, FORTISSIMO 110, PULSAR, MACH EQ4

# Compressors:
    WING COMPRESSOR, WING EXPANDER, BDX 160 COMP, BDX 560 EASY, DRAW MORE COMP, EVEN COMP/LIM, SOUL 9000, SOUL BUS COMP, RED3 COMPRESSOR, 76 LIMITER AMP, LA LEVELER, FAIR KID, ETERNAL BLISS, NO-STRESSOR, WAVE DESIGNER, AUTO RIDER

In order for a wapi program to gain access to plugin parameters, independently from the plugin being installed/loaded at a given slot, the plugin parameter names are being ‘anonymized’ to names p01…pnn, rather than the names that are listed with each single plugin. The actual parameter names for each separate plugin are listed in the plugin description tables later in this document and are preceded with their apparition number in the plugin parameter list; For example, to access the “range“ value of plugin “GATE“ used in channel 03, you would set the token value to CH_3_GATE_P02.

In the small program shown below, we replace the default Gate and Compressor plugins for Channel 1 and set their respective parameters values to arbitrary values. For this example, we use the settings of the AUTO RIDER DYNAMICS gate and Compressor plugins; note that the settings are different for Gate and Compressor, despite the plugin carrying the same name.


    #include <stdio.h>
    #include “wapi.h”
    #include “wext.h”	

    int main () {

        char  wingip[24] = "";

        // we don’t know the IP of our console…
        if (wOpen(wingip)!= WSUCCESS) exit(1);
        printf("WING found at IP: %s\n", wingip);
        printf("Using version %i.%i\n", wVer()/256, wVer());

        wSetTokenString(CH_1_GATE_MDL, "RIDE");	//Auto Rider Dynamics
        wSetTokenFloat(CH_1_GATE_P01, -30.);		// thr
        wSetTokenFloat(CH_1_GATE_P02, 0.);		// tgt
        wSetTokenInt(CH_1_GATE_P03, 20);		// spd
        wSetTokenFloat(CH_1_GATE_P04, 8.);		// ratio
        wSetTokenFloat(CH_1_GATE_P05, 0.5);		// hold
        wSetTokenFloat(CH_1_GATE_P06, 6.0);		// range

        wSetTokenString(CH_1_DYN_MDL, "RIDE");	//Auto Rider Dynamics
        wSetTokenFloat(CH_1_DYN_P01, 50.);		// mix
        wSetTokenFloat(CH_1_DYN_P02, 0.);		// gain
        wSetTokenFloat(CH_1_DYN_P03, -30.);		// thr
        wSetTokenFloat(CH_1_DYN_P04, 0.);		// tgt
        wSetTokenInt(CH_1_DYN_P05, 20);		// spd
        wSetTokenFloat(CH_1_DYN_P06, 4.);		// ratio
        wSetTokenFloat(CH_1_DYN_P07, 0.5);		// hold
        wSetTokenFloat(CH_1_DYN_P08, 3.0);		// range

        return 0;
    }



 
# Effects
Effects nodes are part of the main JSON structure, under the fx.n names, with n: [1…16] representing the 16 effects slots available for simultaneous use in the WIN audio processing. These 16 slots are divided in two sets of slots: 1-8 and slots 9-16 dedicated to premium effects and standard effects, respectively. As one can expect, premium effect slots can be running standard effects too.

As in the case of plugins, the choice of effect is represented by the name (or model) of the effect, as set under the respective “mdl” token; authorized values are: 

# Premium
    NONE, EXTERNAL, HALL REVERB, ROOM REVERB, CHAMBER REVERB, PLATE REVERB, CONCERT REVERB, AMBIENCE, VINTAGE ROOM, VINTAGE REVERB, VINTAGE PLATE, GATED REVERB, REVERSE REVERB, ELAY/REVERB, SHIMMER REVERB, SPRING REVERB, DIMENSION CRS, STEREO CHORUS, STEREO FLANGER, STEREO DELAY, ULTRATAP DELAY, TAPE DELAY, OILCAN DELAYB, BD DELAY, STEREO PITCH, DUAL PITCH, VSS3 REVERB, 

# Standard
    NONE, EXTERNAL, GRAPHIC EQ, PIA 560 GEQ, DOUBLE VOCAL, PRECISION LIMITER, 2-BAND DEESSER, ULTRA ENHANCER, EXCITER, PSYCHO BASS, ROTARY SPEAKER, PHASER, TREMOLO/PANNER, TAPE MACHINE, MOOD FILTER, SUB OCTAVER, RACK AMP, UK ROCK AMP, ANGEL AMP, JAZZ CLEAN AMP, DELUXE AMP, SOUL ANALOGUE, EVEN 88 FORMANT, EVEN 84, FORTISSIMO 110, PULSAR, MACH EQ4

Effects can be used as dedicated inserts at two defined location within the audio path: pre and post xxx.
If an effect is part of a channel insert, assigning the effect to a different channel will remove the effect from its previous channel assignment. In order to create a more traditional effect bus, WING requires to dedicate one of the channels to the operation; Channels that want to use the effect bus can the send their audio (or a part of it) to the channel that carries the effect, creating an effect mix bus that will apply the same effect to several sources mixed into the effect channel and provide the resulting effect as a traditional effect return that can be routed to a bus.

As for the case of plugins, Effect types/engines are represented by their respective model name under the “mdl” tag, enabling the selection (loading) of a specific in one of the 16 available effect slots.

The JSON tree dedicated to effects has the following structure:
 ``` 
    “fx”: {
            “1”: {
                “mdl”: “NONE”,
                “fxmix”: 100
            }
        “2”…“16”: {}
        }
```
In fact, there are a few more, read-only  elements in the actual WING structure of a non-affected effect slot, resulting in the following JSON structure:
``` 
    “fx”: {
            “1”: {
                “mdl”: “NONE”,
                “fxmix”: 100,
                “$esrc”: 0,	external source: [0…400]
                “$emode”: M,	external mode: Mono, Stereo, Mid/Side
                “$a_chn”: 0,	assign channel: [0…76]
                “$a_pos”: 0	assign position: 0, 1]
            }
        “2”…“16”: {}
        }
```
Once an effect is assigned to a slot, the JSON structure for the respective slot is extended to include the parameters for the assigned effect. For example, installing reverb effect “ROOM” in effect slot 5 will result in the following update to the JSON of effect 5:
```
    “fx”: {
        …
            “5”: {
                “mdl”: “ROOM”,
                “fxmix”: 100
                “$esrc”: 0, [0…400]
                “$emode”: M,	[M, ST, M/S]
                “$a_chn”: 0,	[0, 1]
                “$a_pos”: 0,	[0, 1]
                “pdel”:	pre-delay
                “size”:	room size
                “dcy”:		decay
                “mult”:	bass multiplier
                “damp”:	damping
                “lc”:		low cut
                “hc”:		high cut
                “shp”:		shape
                “sprd”:	spread
                “diff”:	diffusion
                “spin”:	spin
                “ecl”:		echo left
                “ecr”:		echo right
                “efl”:		feed left
                “efr”:		feed right
            }
        …
        }
```
Each available effect is a sort of program including a set of dedicated parameters. When choosing a specific effect, the effect program is instantiated in one of the available slots and its parameters are mapped to the main Jason parameters lists for that particular effect slot, thus enabling for example up to 16 different copies  of the same effect to be active on every effect slots, with differentiated parameters for each slot.
The tables below will list the effect names and parameters, and the parameter types associated with each known effect.
In order for a wapi program to gain access to effect parameters, independently from the effect being installed/loaded at a given slot, parameter names are being ‘anonymized’ to names p01…p32, rather than the names that are listed with each single effect. These names listed in the tables below are preceded with their apparition number in the effect parameter list; For example, to access frequency band 125Hz of a Graphics EQ effect loaded at effect slot 12, you would set the token value to FX_12_P09.

To set/instantiate an effect in one of the 16 WING FX slots, just set the model name of the effect; The effect engine will be loaded to the effect slot, discarding a previous one if there was one already. The newly installed effect parameters will become available for tweaking the effect to your settings. Model names can be found behind the tag: “mdl” in the tables below.

In the code example below is shown the instantiation of a graphic equalizer at effect slot 1, and the manipulation of its full set of parameters; In no way an interesting EQ setting, but a simple program example on how to install an effect and set parameters.

    int main() {
        char  wingip[24] = "";
        char *ty[] = {"std", "tru"};
        //
        // we don’t know the IP of our console…
        if (wOpen(wingip)!= WSUCCESS) exit(1);
        printf("WING found at IP: %s\n", wingip);
        printf("Using version %i.%i\n", wVer()/256, wVer());
        //
        int j = 0;
        float f = -15.;
        wSetTokenString(FX_1_MDL, "geq");
        while(1) {
            for (int i = FX_1_P02; i <= FX_1_P32; i++)
                wSetTokenFloat(i, f); // bands 30Hz to 20kHz
            Sleep(1);                    // slow down!
            f += 0.25;
            if (f > 15.) {
                f = -15;
                j ^= 1;
                wSetTokenString(FX_1_P01, ty[j]);  // type
            }
        }
        return 0;
    }

 
# Meters

WING offers many measurement points along the digital audio path; As a result, there are numerous meters. As briefly presented in a table earlier in this document, every Channels, Aux, Bus, Main and Matrix strip offers no less than 8 meters: input left & right, output left & right, gate & dyn key & gain. This alone represents 608 meters that can be retrieved, and there are much more.
The network data path for getting meter values is separated from the main network communication in order to keep things simpler for the programmer and sound engineer.
Meter data is transmitted to a UDP port chosen by the user. When selecting which meters to receive, the user associates an ID to the request, enabling simpler identification of the received data. As soon as a valid meter request is received, WING will send back the respective meter data for 5 seconds, every approximately every 50ms. In order to continue or continuously receive a set of meter data, the user must renew the request for data by issuing a simple renew command containing the ID of the requested meter set.

wapi offers a small set of function calls to help programmers manage meter data. it hides the networking complexity and proposes a simple way of selecting what meter to get back from the digital console. Meter data will be provided back to the application in the form of a buffer of values, decoding data being left to the application.


# int wMeterUDPPort (int wport)
The wMeterUDPPort() API call enable users to select the UDP port WING will send meter data to. It also prepares the wapi internal network for receiving meter data and being able to return data to the user application. wport is a standard UDP port and must be available for receiving data. The function returns WSUCCESS if everything is set correctly or will return an error value if the request was not successful.


# int wSetMetersRequest(int reqID, unsigned char *wMid)
wSetMetersRequest() must be called in order to start receiving meter data. The API associates a request ID reqID to a selection of meters to receive. The request ID helps renewing the request for data and sorting through potentially multiple data sets sent by the console. The wMid parameter holds the selection of meters that can be recovered from WING in an array of 19 bytes. Each bit (from left to right) of the array bytes represents a meter family that can be received from the console, and is shown in the table below:

    byte index  bits    selection
    
    0-5         1-40    Channel 1-40
    6           1-8     Aux 1-8
    7-8         1-16    Bus 1-16
    9           1-4     Main 1-4
    10          1-8	    Matrix 1-8
    11          1-8	    DCA 1-8
    12-13       1-16    FX proc 1-16
    14-15       1-16    Source input 1-16
    16-17       1-11    Output 1-11
    18          1       Monitor
    19          1       RTA

For example, a C source language array declaration as follows will request meters for channels 1 and 40:

    unsigned char	mbits[19] = {0x80, 0, 0, 0, 0x01};	// bytes 6 to 19 are 0   



# int wRenewMeters(int reqID)
The wRenewMeters()  API call is used to renew a previous request for meter data; This function should be called every 5 seconds maximum in order to avoid losing meter data if continuous receiving is expected. The reqID parameter must be previously defined with a call to wSetMetersRequest(). The function returns WSUCCESS if the request is accepted, or will return other error status values otherwise.


# int wGetMeters(unsigned char *buf, int maxlen, int timeout)
wGetMeters() will check if meter data has been received or is available. The call can be blocking or un-blocking depending on the value of timeout. A timeout of 0 will block the application in reading mode until data is available. A non-zero value of timeout, expressed in micro-seconds will return after the provided value and return to the caller with a value of WZERO (0) if no data is available or will return sooner with the actual number of bytes read available in buf. 
The maxlen parameter indicates the maximum number of bytes buf can hold. It is the responsibility of the application to ensure buf is large enough to accept maxlen bytes.
The data returned by the wGetMeters() function is coded as follows:
<reqID><[meter data group][meter data group]  … >
Each meter data group is composed of a number of big-endian short (16bits) ints representing a meter value expressed in 1/256th of a dB. The table below provides the number and origin of each meter data for each of the possible 11 meter groups:
    
    Group name      Contents
    
    channel         input left
    aux             input right
    bus             output left
    main            output right
    matrix          gate key  
                    gate gain
                    dyn key
                    dyn gain

    dca             pre fader left
                    pre fader right
                    post fader left
                    post fader right
                    
    fx              input left
                    input right
                    output left
                    output right
                    state meters (1…6)
                    
    source          source group levels (i.e. local ins: 8 meters)
    
    output          output group levels (i.e. local outs: 8 meters)
    
    monitor	        solo bus left
                    solo bus right
                    mon 1 left
                    mon 1 right
                    mon 2 left
                    mon 2 right
                    
    rta             rta slot meters (120)



Below is an example of buffers received after requesting meter data for channel 1 and using different sources, with Ch 1 fader set to +3dB. 
As received data uses signed 16bits ([-32768…+32767]) and is expressed in 1/256th of dB, the actual meter value can be calculated as <int16>/256. 


                                          gate gate  dyn  dyn
              <reqID>  inL  inR outL outR  key gain  key gain

    W→ 20 B: 00000002 9bb2 9bb2 8000 8000 9b7c 0000 8000 0000 (no input)
                      -100 -100 -128 -128 -100    0 -128    0 
    W→ 20 B: 00000002 f9fb f9fb fcfb fcfb f9fb 0000 ee01 0000 (OSC 1kHz, -6dB)
                        -6   -6   -3   -3   -6    0  -17    0
    W→ 20 B: 00000002 d7fd d7fd dafd dafd d7fd 0000 aa02 0000 (OSC 1kHz, -40dB)
                       -40  -40  -37  -37  -40    0  -85    0


