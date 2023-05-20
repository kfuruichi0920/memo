#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "sock_test.h"

/*-----------------------------------------------------------------------------
*  FUNCTION
-----------------------------------------------------------------------------*/

void print_bind_sock(int sockfd)
{
	struct sockaddr_in s;
	socklen_t sz = sizeof(s);
	getsockname(sockfd, (struct sockaddr *)&s, &sz);

	printf("BIND: %s:%d\n", inet_ntoa(s.sin_addr), (s.sin_port));
}

/**
 * @brief 
 * 
 * @param opt 
 * @return int 
 */
int udp_server(stOpt *opt)
{
	int ret;
	int sock;
	char buf[1024];
	//struct sockaddr_in addr;
	struct addrinfo hints, *res;
	struct group_req greq;
	char strPort[256];
	
	sprintf(strPort, "%d", opt->sPort);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family =AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	ret = getaddrinfo(NULL, strPort, &hints, &res);
	if( ret != 0){
		printf("getaddrinfo : %s\n", gai_strerror(ret));
		return FALSE;
	}
	sock = socket(res->ai_family, res->ai_socktype, 0);
	if( sock < 0 ){
		perror("sock");
		return FALSE;
	}
	ret = bind(sock, res->ai_addr, res->ai_addrlen);
	if( ret != 0 ){
		perror("bind");
		return FALSE;
	}
	freeaddrinfo(res);	

	/* マルチキャストアドレス登録 */
	memset(&hints, 0, sizeof(hints));
	memset(&greq, 0, sizeof(greq));
	hints.ai_family =AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	ret = getaddrinfo(DEFAULT_MULTICAST_IPADDRESS, NULL, &hints, &res);
	if( ret != 0){
		printf("getaddrinfo : %s\n", gai_strerror(ret));
		return FALSE;
	}
	greq.gr_interface = 0;
	memcpy(&greq.gr_group, res->ai_addr, res->ai_addrlen);
	freeaddrinfo(res);
	ret = setsockopt(sock, IPPROTO_IP, MCAST_JOIN_GROUP, (char *)&greq, sizeof(greq));
	if(ret != 0){
		perror("setsockopt");
		return FALSE;
	}

	/* sock情報出力 */
	print_bind_sock(sock);

#if 0
	sock = socket(AF_INET, SOCK_DGRAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(opt->sPort);
	addr.sin_addr.s_addr = INADDR_ANY;	/* すべてのアドレス */

	bind(sock, (struct sockaddr *)&addr, sizeof(addr));
#endif

	memset(buf, 0, sizeof(buf));
	recv(sock, buf, sizeof(buf), 0);
	printf("%s\n", buf);

	close(sock);

	return 0;
}

/**
 * @brief 
 * 
 * @param opt 
 * @return int 
 */
int udp_client(stOpt *opt)
{
	int sock;
	struct sockaddr_in addr;
	int ret;
	int flag = 1;

	/* ソケット生成 */
	sock = socket(AF_INET, SOCK_DGRAM, 0);

	/* 接続先指定用構造体の準備 */
	addr.sin_family = AF_INET;
	addr.sin_port = htons(opt->sPort);
	if(opt->cBroadcast==TRUE){
		inet_pton(AF_INET, DEFAULT_BROADCAST_IPADDRESS, &addr.sin_addr.s_addr);
		ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&flag, sizeof(flag));
		if(ret==-1){
			perror("setsockopt(sock, SOL_SOCKET, SO_BROADCAST)");
			return FALSE;
		}
	}else if(opt->cMulticast==TRUE){
		inet_pton(AF_INET, DEFAULT_MULTICAST_IPADDRESS, &addr.sin_addr.s_addr);
	}else{
		addr.sin_addr.s_addr = inet_addr(opt->strAddr);
	}

	/* 送信 */
	sendto(sock, "Hello", 5, 0, (struct sockaddr *)&addr, sizeof(addr));

	/* ソケットの終了 */
	close(sock);

	return TRUE;
}
