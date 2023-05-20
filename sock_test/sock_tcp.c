#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "sock_test.h"

/*-----------------------------------------------------------------------------
*  DEFINE
-----------------------------------------------------------------------------*/
/**
 * @brief ソケット接続状態のキューに入ることができる、未処理の接続要求最大値。
 * カーネルパラメータでは/proc/sys/net/ipv4/tcp_max_syn_bakcklogで制御可能。
 * 
 */
#define BACKLOG_NUM (10)

/*-----------------------------------------------------------------------------
*  FUNCTION
-----------------------------------------------------------------------------*/

/**
 * @brief 
 * 
 * @param opt 
 * @return int 
 */
int tcp_server(stOpt *opt)
{
	int sock0;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	int len;
	int sock;

	/* ソケット生成 */
	sock0 = socket(AF_INET, SOCK_STREAM, 0);

	/* ソケット設定 */
	addr.sin_family = AF_INET;
	addr.sin_port = htons(opt->sPort);
	addr.sin_addr.s_addr = INADDR_ANY;
	bind(sock0, (struct sockaddr*)&addr, sizeof(addr));

	/* TCPクライアントからの接続要求を受け付ける */
	listen(sock0, BACKLOG_NUM);

	/* TCPクライアントからの接続要求を待つ */
	len = sizeof(client);
	sock = accept(sock0, (struct sockaddr*)&client, &len);

	/* 処理 */
	write(sock, "Hello", 5);

	/* ソケットの終了 */
	close(sock);
	close(sock0);

	return 0;
}

/**
 * @brief 
 * 
 * @param opt 
 * @return int 
 */
int tcp_client(stOpt *opt)
{
	struct sockaddr_in server;
	int sock;
	char buf[32];
	int n;

	/* ソケット生成 */
	sock = socket(AF_INET, SOCK_STREAM, 0);

	/* 接続先指定用構造体の準備 */
	server.sin_family = AF_INET;
	server.sin_port = htons(opt->sPort);
	server.sin_addr.s_addr = inet_addr(opt->strAddr);

	/* サーバに接続 */
	connect(sock, (struct sockaddr *)&server, sizeof(server));

	/* 処理 */
	/* サーバからデータを受信 */
	memset(buf, 0, sizeof(buf));
	n = read(sock, buf, sizeof(buf));

	printf("%d, %s\n", n, buf);

	/* ソケットの終了 */
	close(sock);
}
