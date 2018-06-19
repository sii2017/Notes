## MFC程序的生死因果
以传统的C/SDK撰写Windows 程序，最大的好处是可以清楚看见整个程序的来龙去脉和消息动向，然而这些重要的动线在MFC应用程序中却隐晦不明，因为它们被Application Framework封装起来了。   
本章主要目的除了解释MFC应用程序的长像，也要从MFC源代码中检验出一个Windows程序原本该有的程序进入点（WinMain）、视窗类别注册（RegisterClass）、窗口产生（CreateWindow）、消息循环（Message Loop）、窗口函数（Window Procedure）等等动作，抽丝剥茧彻底了解一个MFC 程序的诞生与结束，以及生命过程。   
> 也许使用MFC不需要了解其中原理，但是了解了其中原理可以更好的操控MFC。   
### 熟记MFC 类别的阶层架构
本章仅仅用到MFC架构中的一小部分，因此我们也仅仅分析一小部分。以下需要熟记：   
![](https://github.com/sii2017/image/blob/master/mfc%E7%9A%84%E4%B8%80%E5%B0%8F%E9%83%A8%E5%88%86.jpg)   
### Precompiled Header
一个应用程序在发展过程中常需要不断地编译。Windows程序包含的标准头文件非常巨大但内容不变，编译器浪费在这上面的时间非常多。Precompiled header 就是将头文件第一次编译后的结果贮存起来，第二次再编译时就可以直接从磁盘中取出来用。这种观念在Borland C/C++ 早已行之，Microsoft 这边则是一直到Visual C++1.0 才具备。    
### 简化的MFC程序架构
![](https://github.com/sii2017/image/blob/master/%E7%AE%80%E5%8C%96%E7%9A%84mfc%E7%A8%8B%E5%BA%8Fhello.jpg)
在这个简单的MFC程序hello中，我们在CwinApp下派生了自己的CMyWinApp以及在CFrameWnd下派生了自己的CMyFramdWnd。   
> 源代码在pdf版，p420。  
### MFC程序的来龙去脉
在MFC程序中与传统的C程序有着一些不同。以下关于这些不同来抽丝剥茧。   
### WinMain程序
在MFC程序中，我们并没有看到WinMain程序，尽管它实际上是存在的只是被包装了起来并且以另外一种形式调用了。   
**WinMain的参数被包装在了CWinApp和它的父类CWinThread中**：  
```c
class CWinApp : public CWinThread   
{   
	HINSTANCE m_hInstance;   
	HINSTANCE m_hPrevInstance;     
	LPTSTR m_lpCmdLine;   
	int m_nCmdShow;	   

	//others...    
}   

class CWinThread : public CCmdTarget   
{   
	CWnd* m_pMainWnd; 　 // main window (usually same AfxGetApp()->m_pMainWnd)   
	CWnd* m_pActiveWnd; // active main window (may not be m_pMainWnd)  
}    
```   
在我们的主程序中，会首先有一个全局变量：  
```c
CMyWinApp theApp;   
```   
我们没有定义CMyWinApp的构造函数，所以它的构造函数就是父类CWinApp的构造函数。  
这个构造函数如上所述配置了WinMain所需要的所有参数。   
theApp 配置完成后，WinMain 登场。我们并未撰写WinMain 程序代码，这是MFC早已准备好并由联结器直接加到应用程序代码中的。其源代码在MFC的WINMAIN.CPP中。  
稍加整理去芜存菁，该WINMAIN函数主要做了以下事情：  
```c
int AFXAPI AfxWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)   
{   
	int nReturnCode = -1;  
	CWinApp* pApp = AfxGetApp();   

	AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow);   

	pApp->InitApplication();   
	pApp->InitInstance();   
	nReturnCode = pApp->Run();  

	AfxWinTerm();   
	return nReturnCode;   
}
```
AfxGetApp 是一个全域函数，用来取得CMyWinApp 对象指针。所以AfxWinMain中实际是这样的：  
```c
CWinApp* pApp = AfxGetApp();  
pApp->InitApplication();   
pApp->InitInstance();   
nReturnCode = pApp->Run();   
```   
根据多态，实际调用的是：   
```c
CWinApp::InitApplication(); //因为CMyWinApp 并没有改写InitApplication   
CMyWinApp::InitInstance(); //因为CMyWinApp 改写了InitInstance   
CWinApp::Run(); 　//因为CMyWinApp 并没有改写Run   
```
这三个函数分别涵盖了：  
1 注册窗口类别。  
2 产生窗口并显示窗口。  
3 运作消息映射。   
三个功能。以下我们再细看。    
### AfxWinInit函数
AfxWinMain中第一个函数就是AfxWinInit。  
它将winmain的四个参数赋值给了pApp。   
```c
CWinApp* pApp = AfxGetApp();  
pApp->m_hInstance = hInstance;   
pApp->m_hPrevInstance = hPrevInstance;  
pApp->m_lpCmdLine = lpCmdLine;   
pApp->m_nCmdShow = nCmdShow;  
```   
并对线程做了一些初始化的处理。   
### InitApplication函数
AfxWinMain中第二个函数是InitApplication。  
### CFrameWnd 取代 WndProc
在c中传统的窗口消息处理函数也不见了，在mfc中我们使用**消息映射**来处理消息。   