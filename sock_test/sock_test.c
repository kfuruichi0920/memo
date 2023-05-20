#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>

#include "sock_test.h"
#include "sock_ext.h"

/*-----------------------------------------------------------------------------
*  DEFINE
-----------------------------------------------------------------------------*/
enum eCHK{
	CHK_TYPE,
	CHK_CLISRV,
	CHK_ILLEGAL,
	CHK_MAX
};

enum eSOCK{
	SOCKTYPE_NONE,
	SOCKTYPE_TCP,
	SOCKTYPE_UDP,
	SOCKTYPE_RAW,
	SOCKTYPE_MAX
};

enum eINFO{
	INFO_IFADDR,
	INFO_MAX
};


/*-----------------------------------------------------------------------------
*  STRUCT
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
*  CONST
-----------------------------------------------------------------------------*/
struct option longopts[] = {
	{"tcp", no_argument, NULL, 't'},
	{"udp", no_argument, NULL, 'u'},
	{"raw", no_argument, NULL, 'r'},
	{"unix", no_argument, NULL, 'U'},
	{"server", no_argument, NULL, 's'},
	{"client", no_argument, NULL, 'c'},
	{"port", required_argument, NULL, 'p'},
	{"addr", required_argument, NULL, 'a'},
	{"promiscas", no_argument, NULL, 'P'},
	{"broadcast", no_argument, NULL, 'B'},
	{"multicast", no_argument, NULL, 'M'},
	{"info", required_argument, NULL, 'i'},
	{"help", no_argument, NULL, 'h'},
	{0,0,0,0}
};

/*-----------------------------------------------------------------------------
*  FUNCTION
-----------------------------------------------------------------------------*/
void print_usage_short(void)
{
	int i;
	char *str_usage[] = {
		"SYNOPSIS",
		"       sock_test [OPTIONS]...",
		"DESCRIPTION",
		"       使用方法は --help オプションの出力参照。",
		};

	for(i=0; i<ARRAY_SIZE(str_usage);i++){
		fprintf(stderr, "%s\n", str_usage[i]);
	}
}
void print_usage(void)
{
	int i;
	char *str_usage[] = {
		"NAME",
		"       sock_test - ソケット動作確認用プログラム",
		"SYNOPSIS",
		"       sock_test [OPTIONS]...",
		"DESCRIPTION",
		"       ",
		"       プロトコル定義は、/etc/protocols を参照。",
		"OPTINOS",
		"       -s, --server",
		"            サーバとして動作する。",
		"       -c, --client",
		"            クライアントとして動作する。[default]",
		"       -t, --tcp",
		"            TCPとして動作する。domain=AF_INET, type=SOCK_STREAM, protocol=0。[default]",
		"       -u, --udp",
		"            TCPとして動作する。domain=AF_INET, type=SOCK_DGRAM, protocol=0。",
		"       -r, --raw",
		"            RAWパケットとして扱う。domain=AF_INET, type=SOCK_RAW, protocol=0。",
		"       -U, --unix",
		"            UNIXドメインソケットとして扱う。",
		"       -a, --addr",
		"            IPアドレスを指定する[127.0.0.1]。",
		"       -p, --port",
		"            ポートを指定する[12345]。",
		"       -B, --broadcast",
		"            ブロードキャスト送信する。",
		"       -M, --multicast",
		"            マルチキャスト送信する。",
		"       -P, --promiscas",
		"            プロミスキャスモードで動作する。サーバ設定の場合のみ有効。",
		"       -i, --info",
		"            情報出力を行う。",
		"            1:論理インタフェース",
		"       -h, --help",
		"            ヘルプを出力する（本出力）。",
		"EXIT STATUS",
		"       0  : 正常終了。",
		"       -1 : 異常終了。",
		"AUTHOR",
		"       K.Furuichi, 2023/5/20",
		"COPYRIGHT",
		"       No License, No Warrenty.",
	};

	for(i=0; i<ARRAY_SIZE(str_usage);i++){
		fprintf(stderr, "%s\n", str_usage[i]);
	}
}

void init_opt(stOpt *opt)
{
	opt->strAddr = DEFAULT_IPADDRESS;
	opt->sPort = DEFAULT_PORT;
	opt->cCliSrv = CLISRV_CLINET;
	opt->cType = SOCKTYPE_TCP;
	opt->cBroadcast = FALSE;
	opt->cMulticast = FALSE;
	opt->cPromiscas = FALSE;
	memset(opt->cInfo, 0, sizeof(opt->cInfo));
	opt->bHelp = FALSE;
}

int main(int argc, char *argv[])
{
	int i;
	stOpt optInfo;
	unsigned char cChkType[CHK_MAX] = {0};
	int opt;
	int longindex;

	/* 初期値設定 */
	init_opt(&optInfo);

	/* オプション解析 */
	if( argc < 2 ){
		print_usage_short();
		return -1;
	}
	while( (opt = getopt_long(argc, argv, "a:p:turUschBMPi:", longopts, &longindex)) != -1 ){
		switch(opt){
			/* アドレス、ポート */
			case 'a':
			optInfo.strAddr = optarg;
			break;
			case 'p':
			optInfo.sPort = atoi(optarg);
			break;

			/* クライアント、サーバ */
			case 'c':
			optInfo.cCliSrv = CLISRV_CLINET;
			cChkType[CHK_CLISRV]++;
			break;
			case 's':
			optInfo.cCliSrv = CLISRV_SERVER;
			cChkType[CHK_CLISRV]++;
			break;

			/* 接続タイプ */
			case 't':
			optInfo.cType = SOCKTYPE_TCP;
			cChkType[CHK_TYPE]++;
			break;
			case 'u':
			optInfo.cType = SOCKTYPE_UDP;
			cChkType[CHK_TYPE]++;
			break;
			case 'r':
			optInfo.cType = SOCKTYPE_RAW;
			cChkType[CHK_TYPE]++;
			break;

			case 'i':
			if(optarg==NULL) cChkType[CHK_ILLEGAL]=2;
			else{
				if(atoi(optarg)==1) optInfo.cInfo[0]=TRUE;
			}
			break;

			case 'h':
			optInfo.bHelp = TRUE;
			break;

			/* 接続タイプ */
			case 'B':
			optInfo.cBroadcast = TRUE;
			break;
			case 'M':
			optInfo.cMulticast = TRUE;
			break;
			case 'P':
			optInfo.cPromiscas = TRUE;
			break;

			default:
			cChkType[CHK_ILLEGAL]=2;
			break;
		}
	}

	/* オプションチェック */
	if(optInfo.bHelp == TRUE){
		print_usage();
		return 0;
	}
	for(i=0; i<CHK_MAX; i++){
		if( 1 < cChkType[i] ){
			print_usage_short();
			return -1;
		}
	}

	/* 接続サマリ */
	fprintf(stdout, "--- 接続設定 ---\n");
	fprintf(stdout, " 動作     :%s\n", optInfo.cCliSrv==CLISRV_CLINET ? "クライアント" : "サーバ");
	fprintf(stdout, " タイプ   :%s\n", optInfo.cType==SOCKTYPE_TCP ? "TCP" : 
									optInfo.cType==SOCKTYPE_UDP ? "UDP" : 
									optInfo.cType==SOCKTYPE_RAW ? "RAW" : "指定なし");
	fprintf(stdout, " IP       :%s\n", optInfo.strAddr);
	fprintf(stdout, " PORT     :%d\n", optInfo.sPort);
	fprintf(stdout, " BROADCAST:%s\n", optInfo.cBroadcast==TRUE ? "TRUE":"FALSE");
	fprintf(stdout, " MULTICAST:%s\n", optInfo.cMulticast==TRUE ? "TRUE":"FALSE");
	fprintf(stdout, " PROMISCAS:%s\n", optInfo.cPromiscas==TRUE ? "TRUE":"FALSE");
	fprintf(stdout, "\n");

	/* 情報出力 */
	if(optInfo.cInfo[0]) get_ifinfo();

	/* 接続設定 */
	switch(optInfo.cType){
		case(SOCKTYPE_TCP):
		if(optInfo.cCliSrv==CLISRV_SERVER) tcp_server(&optInfo);
		else tcp_client(&optInfo);
		break;

		case(SOCKTYPE_UDP):
		if(optInfo.cCliSrv==CLISRV_SERVER) udp_server(&optInfo);
		else udp_client(&optInfo);
		break;

		case(SOCKTYPE_RAW):
		break;

		default:
		break;
	}

	return 0;
}