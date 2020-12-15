# wapi
Current version: 0.10

A C language API for WING Digital Mixing Consoles


![WING personal digital mixing console](https://sites.google.com/site/patrickmaillot/home/wing/Wing%20Default.snap.png?attredirects=0)

This repo is a C code API to WING digital mixing consoles. Ver 0.01 (first release) was capable of get and set *all* WING parameters according to FW 1.06.
This represents more than 28000 parameters! WING FW evolves, so does wapi.

The API uses UDP and TCP networking to establish a connection to the WING console. Typical communication take place over TCP/IP on port 2222.

Parameters can be nodes, int, float or string

Get functions can be attended, timed, or unattended (such as in event programming/loops)
Set functions are... set :)

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
For example the identifier for channel 1 parametric equalizer, setting frequency 2 control, a.k.a “ch.1.peq.2f” in the JSON tree is known as token CH_1_PEQ_2F. The respective data in WING is a float and as written above, it can be modified from an integer, a float or even a string value, and can be returned to the application as an integer, a float of even a string.

Some tokens are read-only; trying to change their value will result in an error returned to the application.

WING tokens are listed in an include file: wapi.h that must be included in your program. The include file also contains the status or error codes that can be returned by the API function calls. The wext.h include file contains function calls definitions to be used.

All function calls are regrouped in a binary library: wapi.lib, that you must include at link time. 
A typical compilation of a source file wtest.c in a Windows environment can be as follows:

    gcc -O3 -Wall -c -fmessage-length=0 -o wtest.o "wtest.c" 
    gcc "-LC:<path to wapi.lib>" -o wtest.exe wtest.o -lwapi -lws2_32

Don’t forget to set the correct path to the wapi.lib file in the above compilation/link lines!
Someone mentioned that the `-mwindows` option might be needed in the link line above, after `-lws2_32`; (to be validated).


WING parameters can be set (or modified) using the wSetxxx API family of calls; Similarly, the parameters can be retrieved from WING using the wGetxxx API family of calls. 


Full documentation on wapi is available as a pdf file at https://sites.google.com/site/patrickmaillot/wing



