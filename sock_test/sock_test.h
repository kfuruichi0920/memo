#ifndef _SOCK_TEST_H
#define _SOCK_TEST_H

/*-----------------------------------------------------------------------------
*  DEFINE
-----------------------------------------------------------------------------*/
#define ARRAY_SIZE(a) (unsigned int)(sizeof(a)/sizeof(a[0]))
#define CLISRV_SERVER (1)
#define CLISRV_CLINET (2)
#define DEFAULT_IPADDRESS "127.0.0.1"
#define DEFAULT_MULTICAST_IPADDRESS "239.129.1.2"
#define DEFAULT_BROADCAST_IPADDRESS "255.255.255.255"
#define DEFAULT_PORT (12345)

enum eBOOL{
	FALSE,
	TRUE
};

/*-----------------------------------------------------------------------------
*  STRUCT
-----------------------------------------------------------------------------*/
typedef struct _stOpt {
	char *strAddr;
	unsigned short sPort;
	unsigned char cCliSrv;
	unsigned char cType;
	unsigned char cBroadcast;
	unsigned char cMulticast;
	unsigned char cPromiscas;
	unsigned char cInfo[16];
 	unsigned char bHelp;
} stOpt;

#endif