#include "SceneGame.h"
#include "Net.h"
#include "Game.h"

int SceneGame::_randomSelf = -1;
int SceneGame::_randomOther = -1;

bool SceneGame::init()
{
	CCLayer::init();

	_menu = CCMenu::create();
	addChild(_menu);

	//开始服务器按钮
	initStartServerButton();

	//连接服务器按钮
	initConnectButton();

	//设置关闭按钮
	CCMenuItem* closeItem = CCMenuItemImage::create("CloseNormal.png", "CloseSelected.png", this, menu_selector(SceneGame::closeGame));
	closeItem->setPosition(ccp(370, -200));
	_menu->addChild(closeItem);

	//开启触摸  用 box 就不需要触摸了
	//setTouchEnabled(true);
	//setTouchMode(kCCTouchesOneByOne);

	//创建随机数
	_randomSelf = CCRANDOM_0_1() * 100;

	return true;
}
//创建服务器开始按钮
void SceneGame::initStartServerButton()
{
	serverItem = CCMenuItemImage::create("zhuji.png", "zhuji2.png");
	_menu->addChild(serverItem);
	serverItem->setTarget(this, menu_selector(SceneGame::startServer));//设置触发函数
	moveBy(ccp(200, 100), serverItem);//设置位置
}
//创建客服端连接按钮
void SceneGame::initConnectButton()
{
	clientItem = CCMenuItemImage::create("kefuji.png","kefuji2.png");
	_menu->addChild(clientItem);
	clientItem->setTarget(this, menu_selector(SceneGame::ConnectServer));//设置触发函数
	moveBy(ccp(200, -100), clientItem);//设置位置
}
//启动服务器
void SceneGame::startServer(CCObject*  sender)
{
	//ConnectServer按钮只能点一次
	serverItem->setEnabled(false);//点进来了，以后再也不能点了
	clientItem->setEnabled(false);

	getHostIP();

	if (Net::getInstance()->startServer(6543))
		scheduleUpdate();//定时器
}
//客服端 连接
void SceneGame::ConnectServer(CCObject* sender)
{
	serverItem->setEnabled(false);//点进来了，以后再也不能点了
	clientItem->setEnabled(false);

	//设置文本  我用 box 代替了这个text
	//_text = CCTextFieldTTF::textFieldWithPlaceHolder("Please input ip", "Arial", 30);
	//addChild(_text);
	//_text->setPosition(ccp(250, 225));

	//创建一个可以编辑的 box
	box = CCEditBox::create(CCSize(250, 50), CCScale9Sprite::create("green_edit.png"));
	addChild(box);
	box->setPosition(ccp(240, 225));
	//增加提示
	box->setPlaceHolder("please input:IP");
	//设置提示字体
	box->setFont("Arial", 30);
	//设置输入的字体颜色
	box->setFontColor(ccc3(0, 0, 255));
	//设置可以输入的长度
	box->setMaxLength(15);
	//弹出软键盘。最后一个参数是，回车键的样子
	box->setReturnType(kKeyboardReturnTypeDone);
	//设置键盘弹出为数字键盘或其他
	box->setInputMode(kEditBoxInputModeAny);

	schedule(schedule_selector(SceneGame::pingIP));
}
//bool SceneGame::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
//{
//	if (_text!=NULL && _text->boundingBox().containsPoint(pTouch->getLocation()))
//		_text->attachWithIME();//调出软键盘
//
//	return false;
//}
void SceneGame::pingIP(float)
{
	/*if (_text == NULL)
		return;
	const char* endIP= _text->getString();*/
	static char _IP[30];

	const char* IP = box->getText();
	if (strlen(IP) >15 || strlen(IP)<8)
		return;

	if (strcmp(_IP, IP) == 0)
		return;
	strcpy(_IP, IP);

	//if (Net::getInstance()->Connect(6543, "127.0.0.1"))//如果连接成功
	if (Net::getInstance()->Connect(6543, IP))//如果连接成功
	{
		if (_randomOther == -1)
		{
			Net::getInstance()->Send((char*)&_randomSelf, 4);//注意,先发,不然会挂死在这里

			//再启动一个定时器，其实没有必要，就当学习一下
			schedule(schedule_selector(SceneGame::RecvRadom));
		}
		unschedule(schedule_selector(SceneGame::pingIP));
		//setTouchEnabled(false);
		box->setVisible(false);
	}
}
//服务端
void SceneGame::update(float)
{
	if (Net::getInstance()->Accept())
	{
		if (_randomOther == -1)//还没有接收到客服端发过来的随机数
		{
			Net::getInstance()->Send((char*)&_randomSelf, 4);
			
			schedule(schedule_selector(SceneGame::RecvRadom));
		}
		unscheduleUpdate();
		CCLog("Connect OK");
	}
}
void SceneGame::RecvRadom(float)
{
	static int recvLen = 0;
	char* buf = (char*)&_randomOther;//随机数
	int ret = Net::getInstance()->Recv(buf + recvLen, 4 - recvLen);

	if (ret <= 0)
		return;

	recvLen += ret;
	if (recvLen != 4)
		return;

	if (_randomSelf > _randomOther || (_randomSelf == _randomOther && Net::getInstance()->isServer()))
	{
		Game::_redStart = true;
	}
	else
	{
		Game::_redStart = false;
	}
	_game = Game::create();
	addChild(_game);
	unschedule(schedule_selector(SceneGame::RecvRadom));
}

void SceneGame::moveBy(CCPoint pt, CCNode* node)
{
	node->setPosition(pt + node->getPosition());
}

void SceneGame::getHostIP()
{
#ifdef WIN32
	char szHost[256];//存放主机名的缓冲区  
	gethostname(szHost, 256);//取得本地主机名称  
	hostent *pHost = ::gethostbyname(szHost);//通过主机名得到地址信息  

	//一个主机可能有多个网卡、多个IP、下面的代码输出所有的IP地址  
	in_addr addr;
	for (int i = 0;; i++)
	{
		//获得地址(网络字节)  
		char *p = pHost->h_addr_list[i];
		if (NULL == p)
			break;//退出循环  

		//将地址拷贝到in_addr结构体中  
		memcpy(&addr.S_un.S_addr, p, pHost->h_length);

		//将in_addr转换为主机字节序  
		char *IP = inet_ntoa(addr);

		CCLabelTTF* label = CCLabelTTF::create(IP, "Arial", 30);
		addChild(label);
		moveBy(ccp(250, 280 - i * 30), label);//设置位置
	}
#else
	int sockfd;
	struct ifconf ifconf;
	struct ifreq *ifreq;
	char buf[512];//缓冲区
	//初始化ifconf
	ifconf.ifc_len = 512;
	ifconf.ifc_buf = buf;
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0)
		return;
	ioctl(sockfd, SIOCGIFCONF, &ifconf); //获取所有接口信息

	//接下来一个一个的获取IP地址
	ifreq = (struct ifreq*)ifconf.ifc_buf;
	for (int i = (ifconf.ifc_len / sizeof(struct ifreq)); i>0; i--)
	{
		if (ifreq->ifr_flags == AF_INET){ //for ipv4
			char* IP = inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr);
			ifreq++;
			if(!strcmp(IP,"127.0.0.1"))
				continue;
			CCLabelTTF* label = CCLabelTTF::create(IP, "Arial", 30);
			addChild(label);
			moveBy(ccp(250, 280 - i * 30), label);//设置位置
		}
	}
#endif
}
void SceneGame::closeGame(CCObject*)
{
	if (Net::getInstance()->isServer())
#ifdef WIN32
		closesocket(Net::getInstance()->getServerSocket());
#else
		close(Net::getInstance()->getServerSocket());
#endif
	else
#ifdef WIN32
		closesocket(Net::getInstance()->getClientSocket());
#else
		close(Net::getInstance()->getClientSocket());
#endif
	exit(0);
}

