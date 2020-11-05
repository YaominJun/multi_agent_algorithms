#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
 
#define BUFLEN 255
/*********************************************************************
*filename: mcastserver.c
*purpose: 演示组播编程的基本步骤，组播服务器端，关键在于加入组
*tidied by: zhoulifa(zhoulifa@163.com) 周立发(http://zhoulifa.bokee.com)
Linux爱好者 Linux知识传播者 SOHO族 开发者 最擅长C语言
*date time:2007-01-25 13:20:00
*Note: 任何人可以任意复制代码并运用这些文档，当然包括你的商业用途
* 但请遵循GPL
*Thanks to: Google.com
*Hope:希望越来越多的人贡献自己的力量，为科学技术发展出力
* 科技站在巨人的肩膀上进步更快！感谢有开源前辈的贡献！
*********************************************************************/
int main(int argc, char **argv)
{
	struct sockaddr_in peeraddr;
	struct in_addr ia;
	int sockfd;
	char recmsg[BUFLEN + 1];
	unsigned int socklen, n;
	struct hostent *group;
	struct ip_mreq mreq;
	int local_port = 7838;
	char remote_ip[32] = "230.1.1.1";

	/* 创建 socket 用于UDP通讯 */
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		printf("socket creating err in udptalk\n");
		exit(1);
	}
 
	/* 设置要加入组播的地址 */
	bzero(&mreq, sizeof(struct ip_mreq));

	if ((group = gethostbyname(remote_ip)) == (struct hostent *) 0) {
		perror("gethostbyname");
		exit(errno);
	}
 
	bcopy((void *) group->h_addr, (void *) &ia, group->h_length);
	/* 设置组地址 */
	bcopy(&ia, &mreq.imr_multiaddr.s_addr, sizeof(struct in_addr));
 
	/* 设置发送组播消息的源主机的地址信息 */
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
 
	/* 把本机加入组播地址，即本机网卡作为组播成员，只有加入组才能收到组播消息 */
	if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq,sizeof(struct ip_mreq)) == -1) {
		perror("setsockopt");
		exit(-1);
	}
 
	socklen = sizeof(struct sockaddr_in);
	memset(&peeraddr, 0, socklen);
	peeraddr.sin_family = AF_INET;

	peeraddr.sin_port = htons(local_port);


	if (inet_pton(AF_INET, remote_ip, &peeraddr.sin_addr) <= 0) {
		printf("Wrong dest IP address!\n");
		exit(0);
	}

 
	/* 绑定自己的端口和IP信息到socket上 */
	if (bind(sockfd, (struct sockaddr *) &peeraddr,sizeof(struct sockaddr_in)) == -1) {
		printf("Bind error\n");
		exit(0);
	}
 
	/* 循环接收网络上来的组播消息 */
	for (;;) {
		bzero(recmsg, BUFLEN + 1);
		n = recvfrom(sockfd, recmsg, BUFLEN, 0,
			(struct sockaddr *) &peeraddr, &socklen);
		if (n < 0) {
			printf("recvfrom err in udptalk!\n");
			exit(4);
		} else {
			/* 成功接收到数据报 */
			recmsg[n] = 0;
			printf("peer:%s", recmsg);
		}
	}
}
