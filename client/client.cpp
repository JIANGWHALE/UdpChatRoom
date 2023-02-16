#if 0
// server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include "../include/protocol.h"

#pragma comment(lib,"ws2_32.lib")

using namespace std;

/**
 * @brief 初始化ws2_32库
*/
void InitWs2_32();

int main()
{
	InitWs2_32();

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
	{
		cout << "sock 创建失败" << endl;
	}

	sockaddr_in siServer = {};
	siServer.sin_family = AF_INET;
	siServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	siServer.sin_port = htons(5555);
	int nLen = 0;

	char szSend[] = "test";
	int nRet = sendto(sock, szSend, sizeof(szSend), 0, (sockaddr*)&siServer, sizeof(siServer));

	char szRecv[255] = {};
	nRet = recvfrom(sock,szRecv,sizeof(szRecv),0, (sockaddr*)&siServer,&nLen);

	closesocket(sock);
}


void InitWs2_32()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		return;
	}

	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		WSACleanup();
		return;
	}

	/* The WinSock DLL is acceptable. Proceed. */

}
#endif

#include <iostream>
#include <windows.h>
#include <winsock2.h>
using namespace std;
int main()
{
	time_t t1 = time(NULL);
	cout << t1 << endl;
	Sleep(2000);
	time_t t2 = time(NULL);
	cout << t2 << endl;
}