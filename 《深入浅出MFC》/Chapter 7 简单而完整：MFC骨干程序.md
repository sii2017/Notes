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
