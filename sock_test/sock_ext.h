#ifndef _SOCK_EXT_H
#define _SOCK_EXT_H

#include "sock_test.h"

extern int tcp_server(stOpt *opt);
extern int tcp_client(stOpt *opt);
extern int udp_server(stOpt *opt);
extern int udp_client(stOpt *opt);

int get_ifinfo(void);

#endif