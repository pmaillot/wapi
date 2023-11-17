/*
 *  wosm:
 *
 *  	©2023 - Patrick-Gilles Maillot
 *  	Nov. 10, 2023
 *
 *  Simple test for getting data sent by Open Sound Measurement to a multicast server
 *  at IP 239.255.42.42, port 49007
 *
 *  The program joins the multicast server group and from then receives UDP data each time
 *  OSM sends its dBFs raw data to the server.
 *
 *  No provisions are made to catch errors or mishaps in the way the two applications work
 *
 */
#include "../wapi/wapi.h"
#include "../wapi/wext.h"
#ifdef _WIN32
    #include <Ws2tcpip.h> // needed for ip_mreq definition for multicast
    #include <Windows.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <time.h>
#endif

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MSGBUFSIZE 2048
char	WIP[32] = "";							// Auto Find WING
char	Wbuffer[128];							// Working buffer for nodes
//
int	fd;
fd_set 	wdfs;
int 	w_status;
struct	timeval		WTimeout = {1, 0};			// UDP non-blocking Read timeout (1 full second)
struct 	sockaddr_in addr;						// Server IP
struct 	ip_mreq mreq;							// Multicast group
char 	msgbuf[MSGBUFSIZE];						// Used to read OSM buffer
int 	nbytes;
u_int 	yes = 1;
//
int main()
{
    char* group = "239.255.42.42";
    int port = 49007;

	if (wOpen(WIP) != WSUCCESS) {
		printf("Error wOpen\n");
		return -1;
	}
	printf("wOpen OK\n");
	fflush(stdout);
	//
	// wapi initialized Windows Socket API, so no need to redo it
    //
    // create what looks like an ordinary UDP socket
	if ((fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) >= 0) {
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &yes, sizeof(yes)) >= 0) {
			memset(&addr, 0, sizeof(addr));						// Clear "From" structure
			addr.sin_family = AF_INET;							// Internet/IP
			addr.sin_addr.s_addr = htonl(INADDR_ANY);			// Any IP address
			addr.sin_port = htons(port);
			if (bind(fd, (struct sockaddr*) &addr, sizeof(addr)) >=0) {
				printf("bind OK\n");
				fflush(stdout);
				mreq.imr_multiaddr.s_addr = inet_addr(group);
			    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
			    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq)) >= 0) {
					sprintf(Wbuffer, "/$ctl.user.1.1.enc.name=A dBFS/SPL,..2.enc.name=B dBFS/SPL,..3.enc.name=C dBFS/SPL,..4.enc.name=Z dBFS/SPL");
					if (wSetNode(Wbuffer) != WSUCCESS) {
						printf("error setnode\n");
						fflush(stdout);
					}
			    	while (1) {
						FD_ZERO (&wdfs);
						FD_SET (fd, &wdfs);
						if ((w_status = select(fd + 1, &wdfs, NULL, NULL, &WTimeout)) > 0) {
							nbytes = recvfrom(fd, msgbuf, MSGBUFSIZE, 0, 0, 0);
							if (nbytes < 0) {
								perror("recvfrom");
								return 1;
							} else {
								msgbuf[nbytes] = '\0';
								char *s;
								// There are better ways to do this, but this is a quick test :-)
								float A, B, C, Z;
								if ((s = strstr(msgbuf, "Fast")) != NULL) {
									sscanf(s+6, "%f", &A);
									s = strstr(s+6, "Fast");
									sscanf(s+6, "%f", &B);
									s = strstr(s+6, "Fast");
									sscanf(s+6, "%f", &C);
									s = strstr(s+6, "Fast");
									sscanf(s+6, "%f", &Z);
									sprintf(Wbuffer, "/$ctl.user.1.1.bu.name=%5.1f,..2.bu.name=%5.1f,..3.bu.name=%5.1f,..4.bu.name=%5.1f", A, B, C, Z);
									if (wSetNode(Wbuffer) != WSUCCESS) {
										printf("error wSetNode\n");
										fflush(stdout);
									}
									sprintf(Wbuffer, "/$ctl.user.1.1.bd.name=%5.1f,..2.bd.name=%5.1f,..3.bd.name=%5.1f,..4.bd.name=%5.1f", 140.+A, 140.+B, 140.+C, 140.+Z);
									if (wSetNode(Wbuffer) != WSUCCESS) {
										printf("error wSetNode\n");
										fflush(stdout);
									}
								}
							}
						} else {
							if (w_status < 0) {
								perror("w_status < 0");
							}
						}
			    	}
			    } else {
			    	perror("membership");
			    }
			} else {
				perror("bind");
			}
		} else {
			perror("socket reuse");
		}
	} else {
		perror("socket create");
	}
	// will never get there if all works correctly; Use Ctrl-C to stop program
	wClose();
    return 0;
}
