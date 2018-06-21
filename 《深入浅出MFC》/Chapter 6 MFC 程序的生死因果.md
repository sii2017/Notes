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
如之前所说，根据多态，实际调用的是：  
```c
CWinApp::InitApplication();   
```   
在这个函数的实现中，做了一些MFC为了内部管理而做的动作。   
### InitInstance
AfxWinMain中第三个函数是InitInstance。  
该函数在CMyWinApp中改写了。  
```c
BOOL CMyWinApp::InitInstance()   
{   
	m_pMainWnd = new CMyFrameWnd();   
	m_pMainWnd->ShowWindow(m_nCmdShow);  
	m_pMainWnd->UpdateWindow();  
	return TRUE;   
}    
```   
> 注意：应用程序一定要改写虚拟函数InitInstance，因为它在CWinApp 中只是个空函数，没有任何内建（预设）动作。   
> ㆒般而言，CMyWinApp只改写CWinApp㆗的InitInstance，通常它不改寫 InitApplication和Run。   
### new CMyFrameWnd()
在改写的InitInstance中下一步调用的new CMyFrameWnd()将引发构造函数，该构造函数是由程序员改写父类的虚函数。   
```c
CMyFrameWnd::CMyFrameWnd   
{    
	Create(NULL, "Hello MFC", WS_OVERLAPPEDWINDOW, rectDefault, NULL, "MainMenu");    
}
```   
其中Create 是CFrameWnd 的成员函数，它将产生一个窗口。但，使用哪一个窗口类别呢？实际上就在该函数中，继续看下去：   
```c
BOOL Create( LPCTSTR lpszClassName,  
LPCTSTR lpszWindowName,   
DWORD dwStyle = WS_OVERLAPPEDWINDOW,   
const RECT& rect = rectDefault,  
CWnd* pParentWnd = NULL,   
LPCTSTR lpszMenuName = NULL,   
DWORD dwExStyle = 0,   
CCreateContext* pContext = NULL )    
{   
	HMENU hMenu = NULL;   
	if (lpszMenuName != NULL)  
	{   
		HINSTANCE hInst = AfxFindResourceHandle(lpszMenuName, RT_MENU);    
		hMenu = ::LoadMenu(hInst, lpszMenuName);//读取菜单
	}   

	m_strTitle = lpszWindowName;    

	CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, pParentWnd->GetSafeHwnd(), hMenu, (LPVOID)pContext);   

	return TRUE;   
}    
```
该函数中先读取了菜单项，然后再次调用了一个CreateEx函数，再继续看进去：  
```c
BOOL CWnd::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU nIDorHMenu, LPVOID lpParam)   
{  
	CREATESTRUCT cs;   
	cs.dwExStyle = dwExStyle;  
	cs.lpszClass = lpszClassName;   
	cs.lpszName = lpszWindowName;  
	cs.style = dwStyle;  
	cs.x = x;   
	cs.y = y;   
	cs.cx = nWidth;   
	cs.cy = nHeight;   
	cs.hwndParent = hWndParent;   
	cs.hMenu = nIDorHMenu;  
	cs.hInstance = AfxGetInstanceHandle();   
	cs.lpCreateParams = lpParam;   

	PreCreateWindow(cs);//下一个看他   
	AfxHookWindowCreate(this);     
	HWND hWnd = ::CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style, cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);    
	//...   
}   

BOOL CFrameWnd::PreCreateWindow(CREATESTRUCT& cs)   
{  
	if (cs.lpszClass == NULL)   
	{   
		AfxDeferRegisterClass(AFX_WNDFRAMEORVIEW_REG);   
		cs.lpszClass = _afxWndFrameOrView;     
	}   
	//...      
}    
```    
其中AfxDeferRegisterClass是一个定义于AFXIMPL.H 中的宏。   
这个宏表示，如果变量afxRegisteredClasses的值显示系统已经注册了fClass这种视窗类别，MFC就啥也不做；否则就调用AfxEndDeferRegisterClass(fClass)准备注册之。afxRegisteredClasses 定义于AFXWIN.H，是一个旗标变量，用来记录已经注册了哪些视窗类别。   
到这里我们就比较清楚了。不同类别的PreCreateWindow 成员函数都是在窗口产生之前一刻被调用，准备用来注册窗口类别。如果我们指定的窗口类别是NULL，那么就使用系统预设类别。    
### 窗口的显示与更新
当我们完成了Create注册并创建了窗口，那么后面就是显示和更新窗口了。   
调用ShowWindow函数令窗口显示出来，并调用UpdateWindow函数令Hello程序送出WM_PAINT消息。   
消息是怎么处理的呢？我们继续看下去。   
### Run()  
绕了一圈再回到AfxWinMain中：  
```c
int AFXAPI AfxWinMain (...)  
{  
	CWinApp* pApp = AfxGetApp();  
	AfxWinInit(...);   
	pApp->InitApplication();     
	pApp->InitInstance();  
	nReturnCode = pApp->Run();//接下来是这里   
	AfxWinTerm();   
}  
```    
Hello 程序进行到这里，窗口类别注册好了，窗口诞生并显示出来了，UpdateWindow 被调用，使得消息队列中出现了一个WM_PAINT 消息，等待被处理。现在，执行的脚步到
达pApp->Run。   
由于多态，pApp指向的Run实际上调用的是CWinApp::Run()。   
经过一层一层的包装，最内部可以找到与c中相同的TranslateMessage和DispatchMessage函数。而关于窗口消息处理函数，我们在之前注册窗口的时候就可以看到：  
```c
wndcls.lpfnWndProc = DefWindowProc;   
```   
> 注意，虽然窗口函数被指定为DefWindowProc 成员函数，但事实上消息并不是被发往该处，而是一个名为AfxWndProc的全域函数去。这其中牵扯到MFC暗中做了大挪移的手脚（利用hook 和subclassing），将在第９章详细讨论。   
   
WinMain 已由MFC 提供，窗口类别已由MFC 注册完成、连窗口函数也都由MFC提供。   
作为程序员，处理消息只需要采用消息映射就可以了，也就是所谓的Message Map 机制。   
### CFrameWnd 取代 WndProc
在c中传统的窗口消息处理函数也不见了，在mfc中我们使用**消息映射**来处理消息。   
基本上Message Map 机制是为了提供更方便的程序接口（例如宏或表格），让程序员很方便就可以建立起消息与处理例程的对应关系。   
MFC 提供给应用程序使用的「很方便的接口」是两组宏。以Hello 的主窗口为例，第一个动作是在HELLO.H 的CMyFrameWnd 加上DECLARE_MESSAGE_MAP：   
```c
class CMyFrameWnd : public CFrameWnd   
{   
public:   
	CMyFrameWnd();   
	afx_msg void OnPaint();  
	afx_msg void OnAbout();   
	DECLARE_MESSAGE_MAP()   
};  
```    
第二个动作是在HELLO.CPP 的任何位置（当然不能在函数之内）使用宏如下：  
```c
BEGIN_MESSAGE_MAP(CMyFrameWnd, CFrameWnd)   
	ON_WM_PAINT()   
	ON_COMMAND(IDM_ABOUT, OnAbout)   
END_MESSAGE_MAP()   
```    
这么一来就把消息WM\_PAINT导到OnPaint函数，把WM\_COMMAND（IDM\_ABOUT）导到OnAbout函数去了。   
但是，单凭一个ON\_WM\_PAINT宏，没有任何参数，如何使WM_PAINT 流到OnPaint 函数呢？   
MFC 把消息主要分为三大类，Message Map机制中对于消息与函数间的对映关系也明定以下三种:    
■ 标准Windows 消息（WM\_xxx）的对映规则：   
![](https://github.com/sii2017/image/blob/master/%E6%A0%87%E5%87%86windows%E6%B6%88%E6%81%AF.jpg)   
■ 命令消息（WM_COMMAND）的一般性对映规则是：   
ON_COMMAND(<id>,<memberFxn>)   
例如：   
```c
ON_COMMAND(IDM_ABOUT, OnAbout)   
ON_COMMAND(IDM_FILENEW, OnFileNew)  
ON_COMMAND(IDM_FILEOPEN, OnFileOpen)   
ON_COMMAND(IDM_FILESAVE, OnFileSave)   
```   
前一个参数对应控件消息ID，后一个参数对应处理的函数。   
■ 「Notification 消息」（由控制组件产生，例如BN_xxx）的对映机制的宏分为好几种（因为控制组件本就分为好几种），以下各举一例做代表：    
![](https://github.com/sii2017/image/blob/master/notification%E6%B6%88%E6%81%AF.jpg)  
各个消息处理函数均应以afx_msg void 为函数型式。    
如果某个消息在Message Map中找不到对映记录，消息会往基础类别流窜，这个消息流窜动作称为「Message Routing」。如果一直窜到最基础的类别仍找不到对映的处理例程，自会有预设函数来处理，就像SDK中的DefWindowProc一样。   
MFC 的CCmdTarget 所衍生下来的每一个类别都可以设定自己的Message Map，因为它们都可能（可以）收到消息。   
### 总结
**程序的诞生：**   
■ Application object 产生，内存于是获得配置，初值亦设立了。   
■ Afx WinMain 执行AfxWinInit，后者又调用AfxInitThread，把消息队列尽量加大到96。   
■ Afx WinMain 执行InitApplication。这是CWinApp 的虚拟函数，但我们通常不改写它。   
■ AfxWinMain 执行InitInstance。这是CWinApp 的虚拟函数，我们必须改写它。  
■ CMyWinApp::InitInstance 'new' 了一个CMyFrameWnd 对象。   
■ CMyFrameWnd 构造式调用Create，产生主窗口。我们在Create 参数中指定的窗口类别是NULL， 于是MFC 根据窗口种类， 自行为我们注册一个名为"AfxFrameOrView42d" 的窗口类别。    
■ 回到InitInstance 中继续执行ShowWindow，显示窗口。   
■ 执行UpdateWindow，于是发出WM_PAINT。   
■ 回到AfxWinMain，执行Run，进入消息循环。   
**程序开始运作：**   
■ 程序获得WM_PAINT 消息（藉由CWinApp::Run 中的::GetMessage 循环）。   
■ WM_PAINT 经由::DispatchMessage 送到窗口函数CWnd::DefWindowProc 中。   
■ CWnd::DefWindowProc 将消息绕行过消息映射表格（Message Map）。    
■ 绕行过程中发现有吻合项目，于是调用项目中对应的函数。此函数是应用程序利用BEGIN_MESSAGE_MAP 和END_MESSAGE_MAP 之间的宏设立起来的。   
■ 标准消息的处理例程亦有标准命名，例如WM_PAINT 必然由OnPaint 处理。    
**以下是程序的死亡：**   
■ 使用者选按【File/Close】，于是发出WM_CLOSE。    
■ CMyFrameWnd 并没有设置WM_CLOSE 处理例程，于是交给预设之处理例程。    
■ 预设函数对于WM_CLOSE 的处理方式是调用::DestroyWindow， 并因而发出WM_DESTROY。    
■ 预设之WM_DESTROY 处理方式是调用::PostQuitMessage，因此发出WM_QUIT。    
■ CWinApp::Run 收到WM_QUIT 后会结束其内部之消息循环， 然后调用ExitInstance，这是CWinApp 的一个虚拟函数。      
■ 如果CMyWinApp改写了ExitInstance ， 那么CWinApp::Run 所调用的就是CMyWinApp::ExitInstance，否则就是CWinApp::ExitInstance。   
■ 最后回到AfxWinMain，执行AfxWinTerm，结束程序。    
### Callback 函数
Callback函数是被操作系统调用的函数，而不是程序员调用的。因此，如果类别的成员函数是一个callback 函数， 你必须声明它为"static"，才能把C++ 编译器加诸于函数的一个隐藏参数this去掉。（详情可参考同目录下BLOG\C++\this指针）   
### 空闲时间（idle time）的处理：OnIdle
在run中有一些关于空闲时间处理的内容。  
```c
int CWinThread::Run()   
{  
	//...  
	for (;;)   
	{    
		while (bIdle &&
		!::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE))   
		{   
			//空闲时间的使用   
			if (!OnIdle(lIdleCount++))   
				bIdle = FALSE; // assume "no idle" state  
		}    
		... // msg loop   
	}   
}   
```   
CThread::OnIdle 做些什么事情呢？CWinApp 改写了OnIdle 函数，CWinApp::OnIdle 又做些什么事情呢？   
从源代码可知，他们做一些维护MFC的工作。   
如果我们希望MFC程序可以处理空闲时间，我们可以在CMyWinApp中改写该函数：   
```c
virtual BOOL OnIdle(LONG lCount);   
```
实际上空闲时间比我们想象的要多。  
### 通用对话框（Common Dialogs）
有些对话框，例如【File Open】或【Save As】对话框，出现在每一个程序中的频率很高。因此，自从Windows 3.1 之后Windows API多了一组通用对话框API函数，系统也多了一个对应的COMMDLG.DLL（32位版则为COMDLG32.DLL）。    
MFC 也支持通用对话框，下面是其类别与其类型：    
CCommonDialog       以下各类别的父类别   
CFileDialog         File对话框（Open 或Save As）   
CPrintDialog        Print对话框   
CFindReplaceDialog  Find and Replace 对话框   
CColorDialog        Color 对话框   
CFontDialog         Font 对话框    
CPageSetupDialog    Page Setup 对话框（MFC 4.0 新增）   
COleDialog          Ole 相关对话框   
![](https://github.com/sii2017/image/blob/master/%E9%80%9A%E7%94%A8%E5%AF%B9%E8%AF%9D%E6%A1%86%E7%B1%BB.jpg)   
在C/SDK 程序中， 使用通用对话框的方式是， 首先填充一块特定的结构如OPENFILENAME，然后调用API 函数如GetOpenFileName。当函数回返，结构中的某些字段便持有了使用者输入的值。    
MFC 通用对话框类别，使用之简易性亦不输Windows API。下面这段码可以激活【Open】对话框并最后获得文件完整路径：   
```c
char szFileters[] = "Text fiels (*.txt)|*.txt|All files (*.*)|*.*||";    
CFileDialog opendlg (TRUE, "txt", "*.txt",
OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);    
if (opendlg.DoModal() == IDOK) 
{
	filename = opendlg.GetPathName();
}
```  
> opendlg 构造式的第一个参数被指定为TRUE，表示我们要的是一个【Open】对话框而不是【Save As】对话框。第二参数"txt" 指定预设扩展名；如果使用者输入的文件没有
扩展名，就自动加上此一扩展名。第三个参数"*.txt" 出现在一开始的【file name】字段中。OFN_ 参数指定文件的属性。第五个参数szFilters 指定使用者可以选择的文件型
态，最后一个参数是父窗口。   
当DoModal 回返，我们可以利用CFileDialog 的成员函数GetPathName取得完整的路径。也可以使用另一个成员函数GetFileName取其不含路径的文件名称，或GetFileTitle 取得既不含路径亦不含扩展名的文件名称。   
   
这便是MFC 通用对话框类别的使用。你几乎不必再从其中衍生出子类别，直接用就好了。   