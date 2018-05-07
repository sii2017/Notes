# 第一章
## Win32基本程序概念   
在学习MFC之前，必要的基础是，对于Windows程序的事件驱动特性的了解（包括消息的产生、获得、分派、判断、处理），以及对C++ 多态（polymorphism）的精确体会。   
![](https://github.com/sii2017/image/blob/master/%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91%E6%B5%81%E7%A8%8B.jpg)   
### 以消息为基础，以事件驱动之
Windows 程序的进行系依靠外部发生的事件来驱动。   
换句话说，程序不断等待（利用一个while循环），等待任何可能的输入，然后做判断，再做适当的处理。上述的「输入」是由操作系统捕捉到之后，以消息形式（一种数据结构）进入程序之中。    
操作系统如何捕捉外围设备（如键盘和鼠标）所发生的事件呢？USER 模块掌管各个外围的驱动程序，它们各有侦测回路。   
如果把应用程序获得的各种「输入」分类，可以分为由硬件装置所产生的消息，如鼠标移动或键盘被按下（放在系统队列system queue中），以及由Windows系统或其它Windows程序传送过来的消息。（放在程序队列application queue中）      
对应用程序来说，消息就是消息，来自哪里或放在哪里其实并没有太大区别，反正程序调用GetMessage API就取得一个消息，程序的生命靠它来推动。   
所有的GUI 系统，包括UNIX的X Window 以及OS/2 的Presentation Manager，都像这样，是以消息为基础的事件驱动系统。      
接受并处理消息的主角就是窗口。每一个窗口都应该有一个函数负责处理消息，程序员必须负责设计这个所谓的「窗口消息处理函数」。    
如果窗口获得一个消息，这个窗口函数必须判断消息的类别，决定处理的方式。（对于某些系统消息，也可以采用不处理来默认处理。）     
以下为窗口程序处理消息的关系图：  
![](https://github.com/sii2017/image/blob/master/message.png)  
### 注册类及窗口创建
在设置好各项窗口类参数后使用RegisterClass()函数来创建窗口类。有一些默认的窗口类如对话框窗口，输入框窗口等可以直接使用不用注册。   
然后使用CreateWindow函数来使用刚刚注册的窗口类，创建一个简单的基本窗口。   
### 消息循环
初始化工作完成后，WinMain进入消息循环：   
```c
while(GetMessage(&msg, ...))  
{   
	TranslateMessage(&msg);//将键盘消息进行转化   
	DispatchMessage(&msg);//分派消息给相应窗口    
}   
```    
其中的TranslateMessage 是为了将键盘消息转化，DispatchMessage 会将消息传给窗口函数去处理。  
消息发生之时，操作系统已根据当时状态，为它标明了所属窗口，而窗口所属之窗口类别又已经标示了窗口函数（也就是wc.lpfnWndProc 所指定的函数)，所以DispatchMessage自有脉络可寻。   
### 窗口消息处理函数
消息循环中的DispatchMessage 把消息分配到哪里呢？它透过USER 模块的协助，送到该窗口的窗口函数去了。窗口函数通常利用switch/case 方式判断消息种类，以决定处置方式。    
由于它是被Windows 系统所调用的（我们并没有在应用程序任何地方调用此函数），所以这是一种call back 函数，意思是指「在你的程序中，被Windows 系统调用」的函数。这些函数虽然由你设计，但是永远不会也不该被你调用，它们是为Windows系统准备的。  
注意，不论什么消息，都必须被处理，所以switch/case 指令中的default: 处必须调用DefWindowProc，这是Windows 内部预设的消息处理函数。   
窗口函数的wParam和lParam的意义，因消息之不同而异。wParam在16位环境中是16位，在32位环境中是32 位。因此，参数内容（格式）在不同操作环境中就有了变化。    
### 消息映射（Message Map)的雏形
以下将模拟MFC的消息映射表格。   
```c   
//定义部分
struct MSGMAP_ENTRY {    
	UINT nMessage;   
	LONG (*pfn)(HWND, UINT, WPARAM, LPARAM);    
};   
#define dim(x) (sizeof(x) / sizeof(x[0]))    

struct MSGMAP_ENTRY _messageEntries[] =   
{  
	WM_CREATE, OnCreate,   
	WM_PAINT, OnPaint,  
	WM_SIZE, OnSize,    
	WM_COMMAND, OnCommand,   
	WM_SETFOCUS, OnSetFocus,   
	WM_CLOSE, OnClose,  
	WM_DESTROY, OnDestroy,    
} ;   

struct MSGMAP_ENTRY _commandEntries =   
{   
	IDM_ABOUT, OnAbout,  
	IDM_FILEOPEN, OnFileOpen,  
	IDM_SAVEAS, OnSaveAs,   
} ;    

//窗口消息处理函数部分  
LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
WPARAM wParam, LPARAM lParam)   
{   
	int i;    
	for(i=0; i < dim(_messageEntries); i++)     
	{    
		if (message == _messageEntries[i].nMessage)   
		//根据位置，通过函数指针来调用struct中不同的函数
		return((*_messageEntries[i].pfn)(hWnd, message, wParam, lParam));   
	}    
	return(DefWindowProc(hWnd, message, wParam, lParam));   
}    


LONG OnCommand(HWND hWnd, UINT message,
WPARAM wParam, LPARAM lParam)   
{    
	int i;    
	for(i=0; i < dim(_commandEntries); i++)    
	{    
		if (LOWORD(wParam) == _commandEntries[i].nMessage)   
		return((*_commandEntries[i].pfn)(hWnd, message, wParam, lParam));   
	}   
	return(DefWindowProc(hWnd, message, wParam, lParam));   
}    

LONG OnCreate(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam)  
{   
	//...   
}  

LONG OnAbout(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam)   
{   
	//...  
}    
```   
这么一来，WndProc 和OnCommand 永远不必改变，每有新要处理的消息，只要在_messageEntries[ ] 和_commandEntries[ ] 两个数组中加上新元素，并针对新消息撰写新的处理例程即可。    
这种观念以及作法就是MFC 的Message Map 的雏形。MFC 把其中的动作包装得更好更精致（当然因此也就更复杂得多），成为一张庞大的消息地图；程序一旦获得消息，就可以按图上溯，直到被处理为止。    
### 程序的生命周期
1. 程序初始化过程中调用CreateWindow，为程序建立了一个窗口，做为程序的萤幕舞台。CreateWindow 产生窗口之后会送出WM_CREATE 直接给窗口函数，后者于是可以在此时机做些初始化动作（例如配置内存、开文件、读初始资料...）。    
2. 程序活着的过程中，不断以GetMessage 从消息贮列中抓取消息。如果这个消息是WM_QUIT，GetMessage 会传回0 而结束while 循环，进而结束整个程序。    
3. DispatchMessage 透过Windows USER 模块的协助与监督，把消息分派至窗口函数。消息将在该处被判别并处理。    
4. 程序不断进行2和3的动作。     
5. 当使用者按下系统菜单中的Close 命令项，系统送出WM_CLOSE。通常程序的窗口函数不栏截此消息，于是DefWindowProc 处理它。    
6. DefWindowProc 收到WM_CLOSE 后， 调用DestroyWindow 把窗口清除。DestroyWindow 本身又会送出WM_DESTROY。     
7. 程序对WM_DESTROY 的标准反应是调用PostQuitMessage。     
8. PostQuitMessage 没什么其它动作，就只送出WM_QUIT 消息，准备让消息循环中的GetMessage 取得，如步骤2，结束消息循环。    