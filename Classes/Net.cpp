#include "Net.h"

Net* Net::theOne = NULL;
Net::Net()
{
	_server = INVALID_SOCKET;//初始化为无效的SOCKET
	_client = INVALID_SOCKET;
	_isServer = false;
	_isListen = false;
	_isConnect = false;
}
//作为服务器接口
bool Net::startServer(unsigned short port)
{
	//创建socket
	_server = socket(AF_INET, SOCK_STREAM, 0);//1.用tcp/ip协议  2.用TCP流协议  3.默认为0
	if (_server == INVALID_SOCKET)
	{
		CCLog("Socket create error");
		return false;
	}
	setNonBlock(_server);//非阻塞

	//填信息
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
#ifdef WIN32
	addr.sin_addr.S_un.S_addr = INADDR_ANY;//任何一个IP
#else
	addr.sin_addr.s_addr = INADDR_ANY;
#endif
	//将端口号和程序绑定
	int ret = bind(_server, (struct sockaddr*)&addr, sizeof(addr));
	if (ret == SOCKET_ERROR)
	{
		CCLog("bind socket error");
		closesocket(_server);
		_server = INVALID_SOCKET;
		return false;
	}
	//设置监听
	listen(_server, 10);//允许10个在这里等待监听

	_isServer = true;//是服务端
	_isListen = true;//是处于监听状态
	_isConnect = false;//还未连接

	return true;
}
//服务端 专门用来接收客服端 的函数
bool Net::Accept()
{
	//判断服务端是否准备好
	if (!_isServer || !_isListen || _isConnect)
		return false;

	//struct sockaddr_in peeraddr;//定义一个对方的地址
	//int peerlen = sizeof(peeraddr);//定义对方的地址长度
	//if ((_client = accept(_server, (struct sockaddr*)&peeraddr, &peerlen)) < 0)//从完成连接队列返回第一个连接，如果已完成连接队列为空，则阻塞
	//	return false;

	//接收一个客服端
	_client = accept(_server, NULL, NULL);
	if (_client == INVALID_SOCKET)//看是否成功接收到客服端
		return false;

	setNonBlock(_server);//非阻塞。毕竟是游戏，不能老阻塞在这里，这也是创建这个函数的原因
	return true;
}
//作为客服端接口
bool Net::Connect(unsigned short port, const char* ipaddr)
{
	//创建客服端socket
	_client = socket(AF_INET, SOCK_STREAM, 0);

	//填信息
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
#ifdef WIN32
	addr.sin_addr.S_un.S_addr = inet_addr(ipaddr);//连接上一个IP
#else
	addr.sin_addr.s_addr = inet_addr(ipaddr);
#endif
	//int ret = connect(_client, (struct sockaddr*)&addr, sizeof(addr));//尝试连接
	//if (ret == SOCKET_ERROR)//看是否连接上了
	//{
	//	CCLog("%s", ipaddr);
	//	closesocket(_client);
	//	_client = INVALID_SOCKET;
	//	return false;
	//}
	if (connect(_client, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		CCLog("%s", ipaddr);
		closesocket(_client);
		return false;
	}
	setNonBlock(_client);//放在connect后面，因为connect是阻塞函数，放在上面会导致 connect 失效

	_isConnect = true;//连接上了之后就不能再次连接了，相当于一个服务端只接收一个客服端
	_isServer = false;//把服务器都设置为false
	_isListen = false;
	return true;
}
//客服端和服务器通用的端口
int Net::Recv(void* buf, int len)//需要传的东西，和长度
{
#if 0
	int alreadyRecvLen = 0;//已经接收到的长度
	/*select()机制中提供一fd_set的数据结构，实际上是一long类型的数组，每一个数组元素都能与一打开的文件句柄,建立联系，建立联系的工作由程序员完成，
	当调用select()时，由内核根据IO状态修改fd_set的内容，由此来通知执行了select()的进程哪一socket或文件发生了可读或可写事件。
	*/
	while (1)
	{
		fd_set set;
		FD_ZERO(&set); /*将set清零使集合中不含任何fd*/
		FD_SET(_client, &set);/*将_client加入set集合*/
		struct timeval tv;
		tv.tv_sec = 0;//秒
		tv.tv_usec = 200;//微秒
		//非阻塞式I/O编程。。select只是检测是否可以发送数据，换句话说，如果你不要效率的话，不需要select函数都是可以的。
		//使用Select就可以完成非阻塞。返回值：准备就绪的描述符数，若超时则返回0，若出错则返回-1。
		int ret = select(-1,//本参数忽略，仅起到兼容作用。
			&set,//指向一组等待可读性检查的套接口。(我们只要可读Client即可)
			NULL,//指向一组等待可写性检查的套接口。
			NULL,//指向一组等待错误检查的套接口。
			&tv);//最多等待时间，对阻塞操作则为NULL。

		if (ret <= 0)
			return -1;

		ret = recv(_client, (char*)buf + alreadyRecvLen, len - alreadyRecvLen, 0);//1.指定接收端套接字描述符  4.一般置0。  
		if (ret > 0)//recv函数返回其实际copy的字节数   copy时出错，那么它返回SOCKET_ERROR    
		{
			alreadyRecvLen += ret;
			if (alreadyRecvLen == len)
				return len;
		}
		else if (ret == 0)//网络中断了，那么它返回0。
			return alreadyRecvLen;
		else
			break;
	}
	return -1;
#else
	setNonBlock(_client);
	return recv(_client, (char*)buf, len, 0);//这样是偷懒的行为
#endif
}
// 发送
int Net::Send(char* buf, int len)
{
#if 0
	int alreadySendLen = 0;//已经发送到的长度
	while (1)
	{
		//向一个已连接的套接口发送数据。不论是客户还是服务器应用程序都用send函数来向TCP连接的另一端发送数据。
		int ret = send(_client, buf + alreadySendLen, len - alreadySendLen, 0);//1.一个用于标识已连接套接口的描述字。
		if (ret > 0)//recv函数返回其实际copy的字节数   
		{
			alreadySendLen += ret;
			if (alreadySendLen == len)
				return len;
		}
		else//copy时出错，网络中断,返回SOCKET_ERROR   
			break;
	}
	return -1;
#else
	setNonBlock(_client);
	return send(_client, buf, len, 0);//这样是偷懒的行为
#endif
}
