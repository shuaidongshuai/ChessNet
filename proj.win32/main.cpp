#include "main.h"
#include "AppDelegate.h"
#include "CCEGLView.h"

USING_NS_CC;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	//初始化windows socket 环境
	WSADATA data;
	DWORD ver = MAKEWORD(2, 2);//告诉windows，用的是什么版本
	WSAStartup(ver, &data);

    // create the application instance
    AppDelegate app;
    CCEGLView* eglView = CCEGLView::sharedOpenGLView();
    eglView->setViewName("ChessNet");
    eglView->setFrameSize(800, 450);
    return CCApplication::sharedApplication()->run();
}
