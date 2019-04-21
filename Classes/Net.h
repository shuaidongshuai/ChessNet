#ifndef __NET_H__
#define __NET_H__

#ifdef WIN32
#include "WinSock2.h"
#pragma comment(lib,"ws2_32.lib")
#else
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>//sockaddr_in
#include <unistd.h>//close
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#endif

#include "cocos2d.h"
#include "SceneGame.h"

USING_NS_CC;

class Net:public CCLayer
{
protected:
	Net();
	bool _isServer;
	bool _isListen;
	bool _isConnect;	
	SOCKET _server;
	SOCKET _client;
	static Net* theOne;
	
	void setNonBlock(SOCKET sock)//设置socket为非阻塞
	{
#ifdef WIN32
		u_long arg = 1;
		ioctlsocket(sock, FIONBIO, &arg);//FIONBIO：允许或禁止套接口s的非阻塞模式。argp指向一个无符号长整型。如允许非阻塞模式则非零，如禁止非阻塞模式则为零。
#else
		int flag = fcntl(sock,F_GETFL,0);
		flag  |= O_NONBLOCK;
		fcntl(sock,F_SETFL,flag);
#endif
	}
public:
	// 成员函数后面加cost表示这个函数不能修改成员变量
	// 实质是用这个const修饰隐藏的this指针
	// 常对象只能调用常函数  const Net* p;
	// p->isServer();
	// p->StartServer(); // error:常量对象只能调用常量成员函数
	bool isServer(/* Net* this */) const
	{
		return _isServer;
	}
	SOCKET getServerSocket()
	{
		return _server;
	}
	SOCKET getClientSocket()
	{
		return _client;
	}
	static Net* getInstance()//单例
	{
		if (theOne == NULL)
			return theOne = new Net;
		return theOne;
	}
	bool startServer(unsigned short port);
	bool Accept();
	bool Connect(unsigned short port, const char* ipaddr);
	int Recv(void* buf, int len);
	int Send(char* buf, int len);
};

#endif
