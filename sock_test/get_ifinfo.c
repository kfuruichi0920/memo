/*
https://www.geekpage.jp/programming/linux-network/getifaddrs.php
*/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>

/*
struct ifaddrs は、/usr/include/ifaddrs.h を参照。
ifa->flgas は、/usr/include/net/if.h を参照。
*/


void print_flag_detail(unsigned int flags)
{
	printf("   [%1s] interface is up.\n", (flags & IFF_UP) ? "*" : "");
	printf("   [%1s] broadcast address valid.\n", (flags & IFF_BROADCAST) ? "*" : "");
	printf("   [%1s] turn on debugging.\n", (flags & IFF_DEBUG) ? "*" : "");
	printf("   [%1s] is a loopback net.\n", (flags & IFF_LOOPBACK) ? "*" : "");
	printf("   [%1s] interface is point-to-point list.\n", (flags & IFF_POINTOPOINT) ? "*" : "");
	printf("   [%1s] avoid use of trailers.\n", (flags & IFF_NOTRAILERS) ? "*" : "");
	printf("   [%1s] resource allocated.\n", (flags & IFF_RUNNING) ? "*" : "");
	printf("   [%1s] no address resolution protocol.\n", (flags & IFF_NOARP) ? "*" : "");
	printf("   [%1s] promiscas mode.\n", (flags & IFF_PROMISC) ? "*" : "");
}

void print_mac_address(struct ifaddrs *ifa)
{
	int fd;
	struct ifreq ifr;

	strncpy(ifr.ifr_ifrn.ifrn_name, ifa->ifa_name, IFNAMSIZ-1);

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	ioctl(fd, SIOCGIFHWADDR, &ifr);
	close(fd);

	printf("  HW ADDR: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
			(unsigned char)ifr.ifr_hwaddr.sa_data[0],
			(unsigned char)ifr.ifr_hwaddr.sa_data[1],
			(unsigned char)ifr.ifr_hwaddr.sa_data[2],
			(unsigned char)ifr.ifr_hwaddr.sa_data[3],
			(unsigned char)ifr.ifr_hwaddr.sa_data[4],
			(unsigned char)ifr.ifr_hwaddr.sa_data[5]);
}

int get_ifinfo(void)
{
  struct ifaddrs *ifa_list;
  struct ifaddrs *ifa;
  int n;
  char addrstr[256], netmaskstr[256];

  /* (1) */
  n = getifaddrs(&ifa_list);
  if (n != 0) {
	return 1;
  }

  /* (2) */
  for(ifa = ifa_list; ifa != NULL; ifa=ifa->ifa_next) {

    /* (3) */
    printf("%s\n", ifa->ifa_name);

    memset(addrstr, 0, sizeof(addrstr));
    memset(netmaskstr, 0, sizeof(netmaskstr));

    if (ifa->ifa_addr->sa_family == AF_INET) {  /* (4) */
      inet_ntop(AF_INET,
        &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr,
        addrstr, sizeof(addrstr));

      inet_ntop(AF_INET,
        &((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr,
        netmaskstr, sizeof(netmaskstr));

      printf("  IPv4: %s netmask %s\n", addrstr, netmaskstr);

    } else if (ifa->ifa_addr->sa_family == AF_INET6) { /* (5) */
      inet_ntop(AF_INET6,
        &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,
        addrstr, sizeof(addrstr));

      inet_ntop(AF_INET6,
        &((struct sockaddr_in6 *)ifa->ifa_netmask)->sin6_addr,
        netmaskstr, sizeof(netmaskstr));

      printf("  IPv6: %s netmask %s\n", addrstr, netmaskstr);
    } else { /* (6) */
      printf("  else:%d\n", ifa->ifa_addr->sa_family);
    }

	print_mac_address(ifa);
    printf("  FLAG: 0x%.8x\n", ifa->ifa_flags);
	print_flag_detail(ifa->ifa_flags);

    printf("\n");
  }

  /* (7) */
  freeifaddrs(ifa_list);

  return 0;
}