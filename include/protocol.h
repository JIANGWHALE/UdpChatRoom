#pragma once
#include <WinSock2.h>
#define NAMELEN 25
#define MSGLEN 255

/**
 * @brief 消息包类型
*/

enum Cmd
{
	C2S_LOGIN,					//客户端上线包
	S2C_LOGIN,					//发送给客户端

	C2S_LOGOUT,					//下线
	S2C_LOGOUT,					//下线

	C2S_PUBCHAT,				//公聊
	S2C_PUBCHAT,				//私聊

	C2S_PRICHAT,				//私聊
	S2C_PRICHAT,				//私聊

	C2S_HEART					//心跳包
};

/**
 * @brief 协议包
*/
struct tagProtoPkg
{
	//默认构造
	tagProtoPkg() {};

	//客户端 心跳包
	tagProtoPkg(Cmd cmd):m_cmd(cmd) {};

	//客户端上线 客户端下线
	tagProtoPkg(Cmd cmd, char* szName):m_cmd(cmd)
	{
		strcpy(m_szName, szName);
	};

	//服务器转发上线 服务器转发下线包
	tagProtoPkg(Cmd cmd, char* szName, sockaddr_in siFrom) :m_cmd(cmd), m_siFrom(siFrom)
	{
		strcpy(m_szName, szName);
	};

	//客户端公聊
	tagProtoPkg(Cmd cmd, char* szName,char *szMsg) :m_cmd(cmd)
	{
		strcpy(m_szName, szName);
		strcpy(m_szMsg, szMsg);
	};

	//服务器转发公聊
	tagProtoPkg(Cmd cmd, char* szName, char* szMsg,sockaddr_in siFrom) :m_cmd(cmd), m_siFrom(siFrom)
	{
		strcpy(m_szName, szName);
		strcpy(m_szMsg, szMsg);
	};

	//客户端私聊
	tagProtoPkg(Cmd cmd, sockaddr_in siTo,char* szName, char* szMsg ) :m_cmd(cmd), m_siTo(siTo)
	{
		strcpy(m_szName, szName);
		strcpy(m_szMsg, szMsg);
	};

	//服务器转发私聊
	tagProtoPkg(Cmd cmd,sockaddr_in siFrom, sockaddr_in siTo, char* szName, char* szMsg) :m_cmd(cmd), m_siTo(siTo),m_siFrom(siFrom)
	{
		strcpy(m_szName, szName);
		strcpy(m_szMsg, szMsg);
	};
	
	Cmd			m_cmd;					//消息类型
	sockaddr_in m_siFrom;				//消息来自谁,ip&port
	sockaddr_in m_siTo;					//消息发送给谁,ip&port（私聊时候使用）
	char		m_szName[NAMELEN];		//用户名
	char		m_szMsg[MSGLEN];		//消息内容
};

/**
 * @brief 运算符重载：用于比较两个sockaddr_in的ip和port是否相等
 * @param siL 
 * @param siR 
 * @return 
*/
bool operator==(sockaddr_in siL, sockaddr_in siR)
{
	return (siL.sin_addr.S_un.S_addr == siR.sin_addr.S_un.S_addr) && (siL.sin_port==siR.sin_port);
}