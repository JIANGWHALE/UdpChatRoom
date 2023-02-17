// server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <list>
#include <mutex>
#include <thread>
#include <winsock2.h>
#include <windows.h>
#include "../include/protocol.h"
#pragma comment(lib,"ws2_32.lib")

/*多端同步测试*/

using namespace std;

/**
 * @brief 初始化ws2_32库
*/
void InitWs2_32();

/**
 * @brief 保存在线客户端信息的结构体
*/
struct tagInfo
{
	tagInfo(char* szName, sockaddr_in si)
	{
		m_si = si;
		strcpy(m_szName, szName);
	}

	tagInfo(char* szName, sockaddr_in si, time_t tiNow)
	{
		m_tHeart = tiNow;
		m_si = si;
		strcpy(m_szName, szName);
	}
	char		m_szName[NAMELEN];			//姓名
	sockaddr_in m_si;						//ip和port
	time_t		m_tHeart;					//上次心跳包时间
};

int main()
{
	InitWs2_32();

	/**
	 * @brief 创建socket
	 * @return sock
	*/
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
	{
		cout << "sock 创建失败" << endl;
	}

	/**
	 * @brief 设置服务器ip和port
	*/
	sockaddr_in siServer = {};
	siServer.sin_family = AF_INET;
	siServer.sin_addr.S_un.S_addr = ADDR_ANY;// inet_addr("127.0.0.1");
	siServer.sin_port = htons(5555);

	/**
	 * @brief 绑定端口
	 * @return
	*/
	int nRet = bind(sock, (sockaddr*)&siServer, sizeof(siServer));
	if (nRet == SOCKET_ERROR)
	{
		cout << "bind error" << endl;
	}
	cout << "start:" << endl;

	list<tagInfo> lst;
	std::mutex mtxLst;

	//启动心跳进程 检测剔除 掉线客户端
	std::thread([&lst, &mtxLst, &sock]() {
		while (true)
		{
			mtxLst.lock();
			for (auto it = lst.begin(); it != lst.end(); it++)
			{
				if (time(NULL) - it->m_tHeart > 5)
				{
					printf("ip:%s port:%d name:%s 掉线了\r\n", inet_ntoa(it->m_si.sin_addr), ntohs(it->m_si.sin_port), it->m_szName);
					auto ClientInfo = *it;
					lst.erase(it);

					cout << "\r\n=========当前在线列表==========" << endl;

					for (auto& info : lst)
					{
						printf("ip:%s port:%d name:%s\r\n", inet_ntoa(info.m_si.sin_addr), ntohs(info.m_si.sin_port), info.m_szName);

						//下线包
						tagProtoPkg pkgOffLine(S2C_LOGOUT, ClientInfo.m_szName, ClientInfo.m_si);
						int nRet = sendto(sock, (char*)&pkgOffLine, sizeof(pkgOffLine), 0, (sockaddr*)&info.m_si, sizeof(info.m_si));
					}
					cout << "=========当前在线列表==========\r\n" << endl;
					break;
				}
			}
			mtxLst.unlock();
		}
		}).detach();

		while (true)
		{
			tagProtoPkg pkgFromClient = {};				//接收消息的缓冲区
			sockaddr_in siClient = {};					//保存客户端信息缓冲区
			int nLen = sizeof(siClient);
			nRet = recvfrom(sock, (char*)&pkgFromClient, sizeof(pkgFromClient), 0, (sockaddr*)&siClient, &nLen);
			mtxLst.lock();
			switch (pkgFromClient.m_cmd)
			{
				//上线功能
			case C2S_LOGIN:
			{
				//日志
				printf("ip:%s port:%d name:%s 上线了\r\n", inet_ntoa(siClient.sin_addr), ntohs(siClient.sin_port), pkgFromClient.m_szName);

				//告诉在线客户端，有新客户端上线了
				for (auto info : lst)
				{
					//构造包
					tagProtoPkg pkgOnline(S2C_LOGIN, pkgFromClient.m_szName, siClient);
					nRet = sendto(sock, (char*)&pkgOnline, sizeof(pkgOnline), 0, (sockaddr*)&info.m_si, sizeof(info.m_si));
				}

				//告诉这个客户端，在线客户端信息
				for (auto info : lst)
				{
					tagProtoPkg pkgOnline(S2C_LOGIN, info.m_szName, info.m_si);
					nRet = sendto(sock, (char*)&pkgOnline, sizeof(pkgOnline), 0, (sockaddr*)&siClient, sizeof(siClient));
				}

				//将这个客户端加入在线客户端
				lst.push_back(tagInfo(pkgFromClient.m_szName, siClient, time(NULL)));


				/*for (auto& info : lst)///测试
				{
					printf("ip:%s port:%d name:%s 在线\r\n", inet_ntoa(info.m_si.sin_addr), ntohs(info.m_si.sin_port), info.m_szName);
				}*/

				break;
			}

			//下线功能
			case C2S_LOGOUT:
			{
				printf("ip:%s port:%d name:%s 下线了\r\n", inet_ntoa(siClient.sin_addr), ntohs(siClient.sin_port), pkgFromClient.m_szName);

				//从在线列表中剔除
				for (auto it = lst.begin(); it != lst.end(); it++)
				{
					if (it->m_si == siClient)
					{
						lst.erase(it);
						break;
					}
				}
				//测试
				/*for (auto info:lst)
				{
					printf("ip:%s port:%d name:%s 当前在线\r\n", inet_ntoa(info.m_si.sin_addr), ntohs(info.m_si.sin_port), info.m_szName);
				}*/


				cout << "\r\n=========当前在线列表==========" << endl;

				//转发给在线客户端
				for (auto info : lst)
				{
					printf("ip:%s port:%d name:%s\r\n", inet_ntoa(info.m_si.sin_addr), ntohs(info.m_si.sin_port), info.m_szName);
					//下线包
					tagProtoPkg pkgOffLine(S2C_LOGOUT, pkgFromClient.m_szName, siClient);
					nRet = sendto(sock, (char*)&pkgOffLine, sizeof(pkgOffLine), 0, (sockaddr*)&info.m_si, sizeof(info.m_si));
				}

				cout << "=========当前在线列表==========\r\n" << endl;

				break;
			}

			//公聊功能
			case C2S_PUBCHAT:
			{
				//日志
				printf("ip:%s port:%d name:%s 说:%s\r\n",
					inet_ntoa(siClient.sin_addr), ntohs(siClient.sin_port),
					pkgFromClient.m_szName,
					pkgFromClient.m_szMsg
				);

				//转发给所有在线客户端
				for (auto info : lst)
				{
					tagProtoPkg pkgPubChat(S2C_PUBCHAT, pkgFromClient.m_szName, pkgFromClient.m_szMsg, siClient);
					nRet = sendto(sock, (char*)&pkgPubChat, sizeof(pkgPubChat), 0, (sockaddr*)&info.m_si, sizeof(info.m_si));
				}

				break;
			}

			//私聊功能
			case C2S_PRICHAT:
			{
				//日志
				printf("ip:%s port:%d name:%s 对 ip:%s port:%d 私聊说:%s\r\n",
					inet_ntoa(siClient.sin_addr), ntohs(siClient.sin_port),
					pkgFromClient.m_szName,
					inet_ntoa(pkgFromClient.m_siTo.sin_addr), ntohs(pkgFromClient.m_siTo.sin_port),
					pkgFromClient.m_szMsg
				);

				//把消息发送给私聊对象
				tagProtoPkg pkgPriChat(S2C_PRICHAT, siClient, pkgFromClient.m_siTo, pkgFromClient.m_szName, pkgFromClient.m_szMsg);
				nRet = sendto(sock, (char*)&pkgPriChat, sizeof(pkgPriChat), 0, (sockaddr*)&pkgFromClient.m_siTo, sizeof(pkgFromClient.m_siTo));

				//把消息转发给自己
				nRet = sendto(sock, (char*)&pkgPriChat, sizeof(pkgPriChat), 0, (sockaddr*)&siClient, sizeof(siClient));

				break;
			}

			//心跳包
			case C2S_HEART:
			{
				//日志
				//printf("ip:%s port:%d 发送了心跳包\r\n", inet_ntoa(siClient.sin_addr), ntohs(siClient.sin_port));

				for (auto& info : lst)
				{
					if (info.m_si == siClient)
					{
						info.m_tHeart = time(NULL);
						break;
					}
				}
				break;
			}
			}
			mtxLst.unlock();

		}
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