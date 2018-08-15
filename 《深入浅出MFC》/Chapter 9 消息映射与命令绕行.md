## 消息映射与命令绕行
在MFC中，与SDK不同的是，UI的命令消息WM_COMMAND是通过消息映射宏与相关的消息处理函数连接。因此不需要像SDK程序中一样使用switch来辨别消息。在MFC中的每一个WM_COMMAND消息都对应着各自的消息处理函数。   
### 消息分类
MFC把消息分为三大类：  
**1 命令消息（WM\_COMMAND) **  
凡是由UI对象产生的消息都是这种命令消息，如来自菜单或者工具栏的按键等等。在SDK中主要通过消息的wParam来识别，但是在MFC中则通过菜单项目的识别码（menu ID）来识别——两种实际上是相同的。   
什么样的类别由资格接受命令消息？凡衍生自CCmdTarget类别都由资格接受。从command target 的字面意义可知，这是命令消息的目的地。也就是说，凡衍生自CCmdTarget 者，它的骨子里就有了一种特殊的机制。把整张MFC 类别阶层图摊开来看，几乎构造应用程序的最重要的几个类别都衍生自CCmdTarget，剩下的不能接收消息的，是像CFile、CArchive、CPoint、CDao（数据库）、Collection Classes（纯粹数据处理）、GDI 等等「非主流」类别。    
**2 标准消息**   
除WM_COMMAND 之外，任何以WM\_ 开头的都算是这一类。任何继承自CWnd 之类别，均可接收此消息。   
**3 Control Notification**    
这种消息由控制组件产生，为的是向其父窗口（通常是对
话盒）通知某种情况。例如当你在ListBox 上选择其中一个项目，ListBox 就会产生LBN_SELCHANGE 传送给父窗口。这类消息也是以WM_COMMAND 形
式呈现。   
### 消息映射表格
「消息映射」是MFC 内建的一个消息分派机制，只要利用数个宏以及固定形式的写法，类似填表格，就可以让Framework 知道，一旦消息发生，该循哪一条路递送。每一个类别只能拥有一个消息映射表格，但也可以没有。   
首先你必须在类别声明档（.H）声明拥有消息映射表格：  
```c
class CScribbleDoc : public CDocument  
{  
	...    
	DECLARE_MESSAGE_MAP()   
};  
```   
然后在类别实作档（.CPP）实现此一表格：   
```c
BEGIN_MESSAGE_MAP(CScribbleDoc, CDocument)  
	//{{AFX_MSG_MAP(CScribbleDoc)   
	ON_COMMAND(ID_EDIT_CLEAR_ALL, OnEditClearAll)  
	ON_COMMAND(ID_PEN_THICK_OR_THIN, OnPenThickOrThin)   
	//...  
	//}}AFX_MSG_MAP   
END_MESSAGE_MAP()   
```  
夹在BEGIN_ 和END_ 之中的宏，除了ON_COMMAND，还可以有许多种。但是标准的Windows消息并不需要由我们指定处理函数的名称。标准消息的处理函数，其名称也是「标准」的（预设的），像是：   
![](https://github.com/sii2017/image/blob/master/%E6%A0%87%E5%87%86%E6%B6%88%E6%81%AF%E7%9A%84%E6%B6%88%E6%81%AF%E5%A4%84%E7%90%86%E5%87%BD%E6%95%B0.jpg)  
随后我们一个一个宏来看。  
### DECLARE_MESSAGE_MAP宏  
```c
// in AFXWIN.H  
#define DECLAR\E_MESSAGE\_MAP() \  
private: \   
	static const AFX\_MSGMAP\_ENTRY \_messageEntries[]; \  
protected: \   
	static AFX\_DATA const AFX\_MSGMAP messageMap; \   
	virtual const AFX_MSGMAP* GetMessageMap() const; \   
//static 修饰词限制了资料的配置，使得每个「类别」仅有一份资料，而不是每一个「对象」各有一份资料。   

//关于AFX_MSGMAP_ENTRY类型  
struct AFX_MSGMAP_ENTRY  
{   
	UINT nMessage; // windows message   
	UINT nCode; // control code or WM_NOTIFY code   
	UINT nID; // control ID (or 0 for windows messages)  
	UINT nLastID; // used for entries specifying a range of control id's   
	UINT nSig; // signature type (action) or pointer to message #   
	AFX_PMSG pfn; // routine to call (or special value)   
};   

//关于AFX_MSGMAP  
struct AFX_MSGMAP  
{  
	const AFX\_MSGMAP* pBaseMap;  
	const AFX\_MSGMAP_ENTRY* lpEntries;   
};   
```  
可以看到AFX_MSGMAP_ENTRY类型的主要作用是，让消息nMessage 对应于函数pfn。  
在AFX_MSGMAP中，pBaseMap 是一个指向「基础类别之消息映射表」的指针，它提供了一个走访整个继承串链的方法，有效地实作出消息映射的继承性。衍生类别将自动地「继承」其基础类别中所处理的消息。   
![](https://github.com/sii2017/image/blob/master/%E6%B6%88%E6%81%AF%E6%98%A0%E5%B0%84%E5%AE%8F.jpg)   
### BEGIN\_ /ON\_/END\_三个宏
在cpp文件中，我们使用了另外三个宏：  
```c
BEGIN_MESSAGE_MAP(CMyView, CView)  
	ON_WM_PAINT()  
	ON_WM_CREATE()   
END_MESSAGE_MAP()   
```   
继续看源代码。  
```c
#define BEGIN_MESSAGE_MAP(theClass, baseClass) \   
	const AFX_MSGMAP* theClass::GetMessageMap() const \  
	{ return &theClass::messageMap; } \   
	AFX_DATADEF const AFX_MSGMAP theClass::messageMap = \  
	{ &baseClass::messageMap, &theClass::_messageEntries[0] }; \   
	const AFX_MSGMAP_ENTRY theClass::_messageEntries[] = \   
	{ \   
#define END_MESSAGE_MAP() \   
	{0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 } \  
	}; \  

#define ON_COMMAND(id, memberFxn) \    
{ WM_COMMAND, CN_COMMAND, (WORD)id, (WORD)id, AfxSig_vv, (AFX_PMSG)memberFxn },   
#define ON_WM_CREATE() \  
{ WM_CREATE, 0, 0, 0, AfxSig_is, \ 
(AFX_PMSG)(AFX_PMSGW)(int (AFX_MSG_CALL CWnd::*)(LPCREATESTRUCT))OnCreate },   
#define ON_WM_DESTROY() \  
{ WM_DESTROY, 0, 0, 0, AfxSig_vv, \  
(AFX_PMSG)(AFX_PMSGW)(void (AFX_MSG_CALL CWnd::*)(void))OnDestroy },  
#define ON_WM_MOVE() \   
{ WM_MOVE, 0, 0, 0, AfxSig_vvii, \   
(AFX_PMSG)(AFX_PMSGW)(void (AFX_MSG_CALL CWnd::*)(int, int))OnMove },    
#define ON_WM_SIZE() \  
{ WM_SIZE, 0, 0, 0, AfxSig_vwii, \   
(AFX_PMSG)(AFX_PMSGW)(void (AFX_MSG_CALL CWnd::*)(UINT, int, int))OnSize },  
#define ON_WM_ACTIVATE() \   
{ WM_ACTIVATE, 0, 0, 0, AfxSig_vwWb, \    
(AFX_PMSG)(AFX_PMSGW)(void (AFX_MSG_CALL CWnd::*)(UINT, CWnd*,  
BOOL))OnActivate },   
#define ON_WM_SETFOCUS() \  
{ WM_SETFOCUS, 0, 0, 0, AfxSig_vW, \   
(AFX_PMSG)(AFX_PMSGW)(void (AFX_MSG_CALL CWnd::*)(CWnd*))OnSetFocus },    
#define ON_WM_PAINT() \   
{ WM_PAINT, 0, 0, 0, AfxSig_vv, \   
(AFX_PMSG)(AFX_PMSGW)(void (AFX_MSG_CALL CWnd::*)(void))OnPaint },   
#define ON_WM_CLOSE() \  
{ WM_CLOSE, 0, 0, 0, AfxSig_vv, \    
(AFX_PMSG)(AFX_PMSGW)(void (AFX_MSG_CALL CWnd::*)(void))OnClose },  
...   
```   
于是这样的宏：  
```c
BEGIN_MESSAGE_MAP(CMyView, CView)  
	ON_WM_CREATE()   
	ON_WM_PAINT()  
END_MESSAGE_MAP()   
```  
展开后是这样的：  
```c
const AFX_MSGMAP* CMyView::GetMessageMap() const  
	{ return &CMyView::messageMap; }  
AFX_DATADEF const AFX_MSGMAP CMyView::messageMap =
{ &CView::messageMap, &CMyView::_messageEntries[0] };   
const AFX_MSGMAP_ENTRY CMyView::_messageEntries[] =  
{   
	{ WM_CREATE, 0, 0, 0, AfxSig_is, \   
	(AFX_PMSG)(AFX_PMSGW)(int (AFX_MSG_CALL CWnd::*)(LPCREATESTRUCT))OnCreate },   
	{ WM_PAINT, 0, 0, 0, AfxSig_vv, \   
	(AFX_PMSG)(AFX_PMSGW)(void (AFX_MSG_CALL CWnd::*)(void))OnPaint },   
	{0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 }  
};  
```   
用图片直观的来表示该宏的结果是这样的：  
![](https://github.com/sii2017/image/blob/master/%E6%B6%88%E6%81%AF%E6%98%A0%E5%B0%84%E5%AE%8F2.jpg)   
如果这套系统用虚函数来实作，将会带来比较大的内存负担，因此没有使用虚函数的特性。   
### 消息的流动
CWnd::WindowProc 调用的OnWndMsg 是用来分辨并处理消息的专职机构：  
如果是命令消息，就交给OnCommand 处理；  
如果是通告消息（Notification），就交给OnNotify 处理；  
WM_ACTIVATE 和WM_SETCURSOR 也都有特定的处理函数；   
而一般的Windows 讯息，就直接在消息映射表中上溯，寻找其归宿（消息处理例程）。   
为什么要特别区隔出命令消息WM_COMMAND 和通告消息WM_NOTIFY 两类呢？因为它们的上溯路径不是那么单纯地只往父类别去，它们可能需要拐个弯。   
源码略。  
### 直线上溯（一般为windows消息）
直线上溯的逻辑很单纯，唯一做的动作就是比对消息映射表，如果吻合就调用表中项目所记录的函数。比对的对象有二，一个是原原本本的消息映射表，另一个是MFC为求快速所设计的一个cache（很复杂不多解释）。比对成功后，调用对应之函数时，有一个巨大的switch/case动作，那是为了确保类型安全。   
以下是CMyView窗口发生的WM\_PAINT消息流动路线：   
![](https://github.com/sii2017/image/blob/master/%E6%B6%88%E6%81%AF%E6%B5%81%E5%8A%A8%E5%9B%BE.jpg)   
### 拐弯上溯（一般为WM_COMMAND命令消息）
当CMyFrameWnd 收到WM_COMMAND，消息唧筒尝试数种绕行路线，使命令消息有机会经过任何一个类别。   
![](https://github.com/sii2017/image/blob/master/%E6%B6%88%E6%81%AF%E7%BB%95%E8%A1%8C.jpg)   
OnCmdMsg是各类别专门用来对付命令消息的函数。每一个「可接受命令消息之对象」在处理命令消息时都会（应该）遵循一个游戏规则：调用另一个目标类别的OnCmdMsg。这才能够将命令消息传送下去。如果说AfxWndProc 是消息流动的「唧筒」，各类别的OnCmdMsg 就是消息流动的「转辙器」。  
以下举一个具体例子。假设命令消息从Scribble 的【Edit/Clear All】发出，其处理常式位在CScribbleDoc，下面是这个命令消息的流浪过程：   
1. MDI 主窗口（ CMDIFrameWnd） 收到命令消息WM_COMMAND， 其ID 为ID_EDIT_CLEAR_ALL。   
2. MDI 主窗口把命令消息交给目前作用中的MDI 子窗口（CMDIChildWnd）。   
3. MDI 子窗口给它自己的子窗口（也就是View）一个机会。   
4. View 检查自己的Message Map。   
5. View 发现没有任何处理例程可以处理此命令消息，只好把它传给Document。    
6. Document 检查自己的Message Map，它发现了一个吻合项：   
```c
BEGIN_MESSAGE_MAP(CScribbleDoc, CDocument)   
	ON_COMMAND(ID_EDIT_CLEAR_ALL, OnEditClearAll)  
	...  
END_MESSAGE_MAP()   
```
于是调用该函数，命令消息的流动路线也告终止。   
如果上述的步骤6 仍没有找到处理函数，那么就：   
7. Document 把这个命令消息再送到Document Template对象去。   
8. 还是没被处理，于是命令消息回到View。  
9. View 没有处理，于是又回给MDI 子窗口本身。  
10. 传给CWinApp 对象-- 无主消息的终极归属。   
![](https://github.com/sii2017/image/blob/master/%E6%B6%88%E6%81%AF%E7%BB%95%E8%A1%8C2.jpg)   
