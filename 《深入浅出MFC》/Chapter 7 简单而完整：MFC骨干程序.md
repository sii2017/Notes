## 简单而完整：MFC骨干程序
上一章的Hello范例只用了三个MFC类别（CWinApp、CFrameWnd 和CDialog）。  
这一章将会看一个完整的MFC骨干程序，其中包括丰富的UI 对象（如工具栏、状态列）的生成，以及很重要的Document/View 架构观念。   
### 熟记MFC类别阶层架构
同上一章，以下表格需熟记。   
![](https://github.com/sii2017/image/blob/master/MFC%E6%9E%B6%E6%9E%84.jpg)   
### Document/View 支撑你的应用程序
Document/View 是MFC 进化为Application Framework 的灵魂。   
在MFC中，Document为各种数据资料，view为交互显示界面。   
MFC的CDocument已经把空壳做好。它可以内嵌其它对象（用来处理基层数据类型如串行、数组等等），所以程序员可以在Document中拼拼凑凑出实际想要表达的文件完整格式。   
CDocument的另一价值在于它搭配了另一个重要的类别：CView。   
CView 就是为了资料的表现而设计的。除了负责显示，View 还负责程序与使用者之间的交谈接口。使用者对资料的编辑、修改都需仰赖窗口上的鼠标与键盘动作才得完成，这些消息都将由View 接受后再通知Document。   
Document/View 的价值在于，这些MFC 类别已经把一个应用程序所需的「数据处理与显示」的函数空壳都设计好了，这些函数都是虚拟函数，所以你可以（也应该）在衍生类别中改写它们。    
有关文件读写的动作在CDocument的**Serialize函数**进行，有关画面显示的动作在CView的**OnDraw或OnPaint函数**进行。当我为自己衍生两个类别CMyDoc和CMyView，我只要把全付心思花在CMyDoc::Serialize和CMyView::OnDraw 身上，其它琐事一概不必管，整个程序自动会运作得好好的。   
### 骨干程序使用哪些MFC类别？
如果把标准图形接口（工具栏和状态列）以及Document/View考虑在内，一个标准的MFC MDI 程序使用这些类别：    
![](https://github.com/sii2017/image/blob/master/MFC%E9%AA%A8%E5%B9%B2%E7%A8%8B%E5%BA%8F%E4%BD%BF%E7%94%A8%E7%9A%84%E7%B1%BB.png)   
![](https://github.com/sii2017/image/blob/master/MFC%E9%AA%A8%E5%B9%B2%E7%B1%BB%E5%AF%B9%E5%BA%94%E7%9A%84%E5%AF%B9%E8%B1%A1.png)   
代码书中有，笔记中简述一下：  
前几个动作，代码与之前的hello程序一致。直到改写InitInstance 这个虚拟函数。  
在InitInstance中：  
1 new 一个CMultiDocTemplate对象，此对象规划Document、View 以及Document Frame窗口三者之关系。   
2 new 一个CMyMDIFrameWnd对象，做为主窗口（框架）对象。   
3 调用LoadFrame，产生主窗口并加挂菜单等诸元，并指定窗口标题、文件标题、文件档扩展名等（关键在IDR_MAINFRAME 常数）。LoadFrame 内部将调用Create，后者将调用CreateWindowEx，于是触发WM_CREATE 消息。由于我们曾于CMainFrame 之中拦截WM_CREATE（利用ON_WM_CREATE 宏），所以WM_CREATE 产生之际Framework 会调用OnCreate。   
回到InitInstance，执行ShowWindow 显示窗口。   
当程序运行期间，产生的消息经由Message Routing 机制，在各类别的Message Map 中寻求其处理例程。   
比如点击open，WM_COMMAND/ID_FILE_OPEN 消息将由CWinApp::OnFileOpen 函数处理。此函数由MFC提供，它在显示过【File Open】对话框后调用Serialize 函数。   
> 我们可以改写Serialize 函数以进行我们自己的文件读写动作。     
   
WM_COMMAND/ID_APP_ABOUT 消息将由OnAppAbout 函数处理。OnAppAbout 函数利用CDialog 的性质很方便地产生一个对话框。   
### Document Template的意义
Document Template 是一个全新的观念。    
稍早已提过Document/View的概念，它们互为表里。View 本身虽然已经是一个窗口，其外围却必须再包装一个外框窗口做为舞台。这样的切割其实是为了让View 可以非常独
立地放置于「MDI Document Frame 窗口」或「SDI Document Frame 窗口」或「OLE Document Frame 窗口」等各种应用之中。也可以说，Document Frame 窗口是View 窗口的一个容器。**资料的内容、资料的表象、以及「容纳资料表象之外框窗口」三者是一体的**，换言之，程序每打开一份文件（资料），就应该产生三份对象：   
1. 一份Document 对象，   
2. 一份View 对象，   
3. 一份CMDIChildWnd 对象（做为外框窗口）    
这三份对象由一个所谓的Document Template 对象来管理。    
让这三份对象产生关系的关键在于CMultiDocTemplate：   
```c
BOOL CScribbleApp::InitInstance()  
{    
	CMultiDocTemplate* pDocTemplate;  
	pDocTemplate = new CMultiDocTemplate(  
		IDR_SCRIBTYPE,   
		RUNTIME_CLASS(CScribbleDoc),   
		RUNTIME_CLASS(CChildFrame),  
		RUNTIME_CLASS(CScribbleView));   
	AddDocTemplate(pDocTemplate);   
}   
```   
如果程序支持不同的资料格式（例如一为TEXT，一为BITMAP），那么就需要不同的Document Template：   
```c
BOOL CMyWinApp::InitInstance()   
{   
	CMultiDocTemplate* pDocTemplate;   
	pDocTemplate = new CMultiDocTemplate(  
		IDR_TEXTTYPE,   
		RUNTIME_CLASS(CTextDoc),   
		RUNTIME_CLASS(CChildFrame),   
		RUNTIME_CLASS(CTextView));   
	AddDocTemplate(pDocTemplate);   
	pDocTemplate = new CMultiDocTemplate(   
		IDR_BMPTYPE,  
		RUNTIME_CLASS(CBmpDoc),   
		RUNTIME_CLASS(CChildFrame),   
		RUNTIME_CLASS(CBmpView));  
	AddDocTemplate(pDocTemplate);   
}  
```  
Document Template 接受了三种类别的CRuntimeClass 指针，于是每当使用者打开一份文件，Document Template 就能够根据「类别型录网」（第３章所述），动态生
成出三个对象（document、view、document frame window）。   
### 主窗口的诞生
![](https://github.com/sii2017/image/blob/master/MFC%E4%B8%BB%E7%AA%97%E5%8F%A3%E7%9A%84%E8%AF%9E%E7%94%9F.jpg)   
窗口产生之际会发出WM_CREATE消息，因此CMainFrame::OnCreate 会被执行起来，那里将进行工具栏和状态列的建立工作（稍后描述）。LoadFrame 函数的参数（本例为IDR_MAINFRAME） 用来设定窗口所使用的各种资源。   
### 工具栏和状态列的诞生（Toolbar & Status bar）
工具栏和状态列分别由CToolBar 和CStatusBar 掌管。两个对象隶属于主窗口，所以我们在CMainFrame 中以两个变量（事实上是两个对象）表示之：   
```c
class CMainFrame : public CMDIFrameWnd    
{    
protected: // control bar embedded members   
	CStatusBar m_wndStatusBar;   
	CToolBar m_wndToolBar;   
};    
```  
当然这个时候，对象只是对象，实际的工具栏和状态列还并未建立起来。   
主窗口产生之际立刻会发出WM_CREATE，我们应该利用这时机把工具栏和状态列建立起来。为了拦截WM_CREATE，首先需在Message Map 中设定「映射项目」：  
```c
BEGIN_MESSAGE_MAP(CMyMDIFrameWnd, CMDIFrameWnd)   
	ON_WM_CREATE()   
END_MESSAGE_MAP()    
```   
ON_WM_CREATE 这个宏表示，只要WM_CREATE 发生，我的OnCreate 函数就应该被调用。下面是由AppWizard 产生的OnCreate 标准动作：   
```c
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)  
{  
	//something else   
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))    
	{    
		TRACE0("Failed to create toolbar\n");   
		return -1; // fail to create    
	}     

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))   
	{    
		TRACE0("Failed to create status bar\n");   
		return -1; // fail to create   
	}  

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);    

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);   

	EnableDocking(CBRS_ALIGN_ANY);  
	DockControlBar(&m_wndToolBar);  

	return 0;  
}   
```   
其中有四个动作与工具栏和状态列的产生及设定有关：   
1 m_wndToolBar.Create(this) 表示要产生一个隶属于this（也就是目前这个对象，也就是主窗口）的工具栏。     
2 m_wndToolBar.LoadToolBar(IDR_MAINFRAME) 将RC 档中的工具栏资源载入。   
3 m_wndStatusBar.Create(this) 表示要产生一个隶属于this 对象（也就是目前这个对象，也就是主窗口）的状态列。  
4 m_wndStatusBar.SetIndicators(,...) 的第一个参数是个数组；第二个参数是数组元素个数。所谓Indicator 是状态列最右侧的「指示窗口」，用来表示大写键、数字键等的On/Off状态。   
### 鼠标拖放（Drag and Drop）
MFC 程序很容易拥有Drag and Drop 功能。意思是，你可以从Shell中以鼠标拉动一个文件，拖到你的程序中，你的程序因而打开此文件并读其内容，将内容放到一个Document Frame 窗口中。甚至，使用者在Shell 中以鼠标对某个文件文件快按两下，也能激活你这个程序，并自动完成开档，读档，显示等动作。   
```c
BOOL CScribbleApp::InitInstance()   
{    
	// Enable drag/drop open  
	m_pMainWnd->DragAcceptFiles();   
	// Enable DDE Execute open   
	EnableShellOpen();  
	RegisterShellFileTypes(TRUE);   
}   
```    
这三个函数的用途如下：   
```c
CWnd::DragAcceptFile(BOOL bAccept=TRUE);    
```  
参数TRUE表示你的主窗口以及每一个子窗口都愿意接受来自Shell的拖放文件。CFrameWnd内有一个OnDropFiles成员函数，负责对WM_DROPFIELS消息做出反应，它会通知application对象的OnOpenDocument（此函数将在第８章介绍），并夹带被拖放的文件的名称。   
```c
CWinApp::EnableShellOpen();    
```   
当使用者在Shell 中对着本程序的文件文件快按两下时，本程序能够打开文件并读内容。如果当时本程序已执行，Framework不会再执行起程序的另一副本，而只是以DDE（Dynamic Data Exchange，动态资料交换）通知程序把文件读进来。DDE 处理例程内建在CDocManager之中。也由于DDE的能力，你才能够很方便地把文件图标拖放到打印机图标上，将文件打印出来。通常此函数后面跟随着RegisterShellFileTypes。    
```c
CWinApp::RegisterShellFileTypes();    
```   
此函数将向Shell 注册本程序的文件型态。有了这样的注册动作，使用者在Shell 的双击动作才有着力点。这个函数
搜寻Document Template 串行中的每一种文件类型，然后把它加到系统所维护的registry（登录数据库）中。   
在传统的Windows 程序中，对Registry的注册动作不外乎两种作法，一是准备一个.reg 档，由使用者利用Windows 提供的一个小工具regedit.exe，将.reg合并到系统的Registry 中。第二种方法是利用::RegCreateKey、::RegSetValue
等Win32函数，直接编辑Registry。MFC 程序的作法最简单，只要调用CWinApp::RegisterShellFileTypes 即可。    
### 消息映射（Message Map）
在本章的Scribble 源代码中可以看到各类别的Message Map。   
```c
BEGIN_MESSAGE_MAP(CScribbleApp, CWinApp)  
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)   
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)  
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)   
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)   
END_MESSAGE_MAP()   
```
除了ID_APP_ABOUT 是由我们自己设计一个OnAppAbout 函数处理之，其它三个消息都交给CWinApp 成员函数去处理，因为那些动作十分制式，没什么好改写的。  
### 标准菜单File / Edit / View / Window / Help
MFC有着不少组件，如之前所说他们大部分已经由MFC准备好，我们可以使用默认的，如果有需要的话也可以进行改写。  
在这些消息对应的函数中，有一部分是已经“预有关联”的，即我们不需要在message map中拦截该消息并进行关联，他们也会由MFC自动调用相关的处理函数进行处理。另一部分没有关联的，则需要由我们在message map中拦截，并绑定需要处理的函数进行处理。   
![](https://github.com/sii2017/image/blob/master/MFC%E7%BB%84%E4%BB%B6%E9%A2%84%E6%9C%89%E5%85%B3%E8%81%941.jpg)   
![](https://github.com/sii2017/image/blob/master/MFC%E7%BB%84%E4%BB%B6%E9%A2%84%E6%9C%89%E5%85%B3%E8%81%942%20.jpg)   
如上可以看到，如new，open，我们需要在消息映射中进行关联函数才能使用，而close，save，exit我们则不需要去管它，他在程序创建之初就已经关联好了。   
### 对话框
在Scribble中可以激活许多对话框，前一节提了许多。唯一要程序员自己动手写代码的的只有About对话框。   
首先为了拦截WM_COMMAND 的ID_APP_ABOUT 项目，首先我们必须设定其Message Map:  
```c
BEGIN_MESSAGE_MAP(CScribbleApp, CWinApp)  
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)   
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)  
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)   
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)   
END_MESSAGE_MAP()   
```   
当消息送来，就由OnAppAbout 处理：   
```c
void CScribbleApp::OnAppAbout()   
{   
	CAboutDlg aboutDlg;  
	aboutDlg.DoModal();   
}  
```   
> 比之于SDK 程序中的对话框，这真是方便太多了。传统SDK 程序要在RC 文件中定义对话框模板，在C程序中设计对话框函数。现在只需从CDialog 衍生出一个类别，然后产生该类别之对象，并指定RC文件中的对话框面板资源，再调用对话框对象的DoModal成员函数即可。   
### 改用CEditView
Scribble step0 除了把一个应用程序的空壳做好，不能再贡献些什么。如果我们在AppWizard 步骤六中把CScribbleView 的基础类别从CView 改为CEditView，就可以具备文字编辑能力。   
它所使用的窗口是Windows 的标准控制组件之一Edit，其SerializeRaw 成员函数可以把Edit 控制组件中的raw text （而非「对象」所持有的资料）写到文件中。当我们在AppWizard 步骤六选择了它，程序代码中所有的CView 统统变成CEditView，而最重要的两个虚拟函数则变成：  
```c
void CScribbleDoc::Serialize(CArchive& ar)  
{   
	((CEditView*)m_viewList.GetHead())->SerializeRaw(ar);   
}    
void CScribbleView::OnDraw(CDC* pDC)  
{    
	CScribbleDoc* pDoc = GetDocument();  
	ASSERT_VALID(pDoc);   
}   
```
就这样，我们不费吹灰之力获得了一个多窗口的文字编辑器，并拥有读写档能力以及预视能力。   
### 小结
本章内容不多，在使用MFC的几个类创造了一个可以MDI窗口。该窗口可以很方便的获得一个窗口应该有的基本功能，如open，save等等。另外通过增加MFC的一些固有的类，可以使窗口轻松的获得工具栏状态栏等。最后当我们在AppWizard 步骤六中选择CEditView，则该软件能直接变成一个多窗口的文字编辑器。   
总一句话来概括本章的创建窗口的过程：不费吹灰之力。这就是MFC的妙用。   
ps：在2018年的今天还想不费吹灰之力显然是不显示的，大部分时间我们在使用MFC的时候都需要更改大部分的内容已达到一个软件的个性化显示。显然这是一个进步。   