#pragma once
#include <WinSock2.h>
#define NAMELEN 25
#define MSGLEN 255

/**
 * @brief ��Ϣ������
*/

enum Cmd
{
	C2S_LOGIN,					//�ͻ������߰�
	S2C_LOGIN,					//���͸��ͻ���

	C2S_LOGOUT,					//����
	S2C_LOGOUT,					//����

	C2S_PUBCHAT,				//����
	S2C_PUBCHAT,				//˽��

	C2S_PRICHAT,				//˽��
	S2C_PRICHAT,				//˽��

	C2S_HEART					//������
};

/**
 * @brief Э���
*/
struct tagProtoPkg
{
	//Ĭ�Ϲ���
	tagProtoPkg() {};

	//�ͻ��� ������
	tagProtoPkg(Cmd cmd):m_cmd(cmd) {};

	//�ͻ������� �ͻ�������
	tagProtoPkg(Cmd cmd, char* szName):m_cmd(cmd)
	{
		strcpy(m_szName, szName);
	};

	//������ת������ ������ת�����߰�
	tagProtoPkg(Cmd cmd, char* szName, sockaddr_in siFrom) :m_cmd(cmd), m_siFrom(siFrom)
	{
		strcpy(m_szName, szName);
	};

	//�ͻ��˹���
	tagProtoPkg(Cmd cmd, char* szName,char *szMsg) :m_cmd(cmd)
	{
		strcpy(m_szName, szName);
		strcpy(m_szMsg, szMsg);
	};

	//������ת������
	tagProtoPkg(Cmd cmd, char* szName, char* szMsg,sockaddr_in siFrom) :m_cmd(cmd), m_siFrom(siFrom)
	{
		strcpy(m_szName, szName);
		strcpy(m_szMsg, szMsg);
	};

	//�ͻ���˽��
	tagProtoPkg(Cmd cmd, sockaddr_in siTo,char* szName, char* szMsg ) :m_cmd(cmd), m_siTo(siTo)
	{
		strcpy(m_szName, szName);
		strcpy(m_szMsg, szMsg);
	};

	//������ת��˽��
	tagProtoPkg(Cmd cmd,sockaddr_in siFrom, sockaddr_in siTo, char* szName, char* szMsg) :m_cmd(cmd), m_siTo(siTo),m_siFrom(siFrom)
	{
		strcpy(m_szName, szName);
		strcpy(m_szMsg, szMsg);
	};
	
	Cmd			m_cmd;					//��Ϣ����
	sockaddr_in m_siFrom;				//��Ϣ����˭,ip&port
	sockaddr_in m_siTo;					//��Ϣ���͸�˭,ip&port��˽��ʱ��ʹ�ã�
	char		m_szName[NAMELEN];		//�û���
	char		m_szMsg[MSGLEN];		//��Ϣ����
};

/**
 * @brief ��������أ����ڱȽ�����sockaddr_in��ip��port�Ƿ����
 * @param siL 
 * @param siR 
 * @return 
*/
bool operator==(sockaddr_in siL, sockaddr_in siR)
{
	return (siL.sin_addr.S_un.S_addr == siR.sin_addr.S_un.S_addr) && (siL.sin_port==siR.sin_port);
}