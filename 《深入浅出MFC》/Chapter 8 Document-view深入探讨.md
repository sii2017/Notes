## Document-View深入探讨
### 为什么需要Document-View
MFC 之所以为Application Framework，最重要的一个特征就是它能够将管理资料的程序码和负责资料显示的程序代码分离开来，这种能力由MFC的Document/View提供。   
即由Document负责数据，由View负责显示，由Frame负责UI。   
### Document
Document负责的是数据，我们需要从CDocument派生一个自己的类出来，并且改写负责文件读写的Serialize虚函数，比如：   
```c
class CScribbleDoc : public CDocument  
{  
	DECLARE_DYNCREATE(CScribbleDoc)    
	//...   
	virtual void Serialize(CArchive& ar);   
	DECLARE_MESSAGE_MAP()   
};  
void CScribbleDoc::Serialize(CArchive& ar)    
{  
	if (ar.IsStoring())   
	{   
	// TODO: add storing code here    
	}   
	else   
	{   
	// TODO: add loading code here   
	}   
}   
```   
由于CDocument 衍生自CObject，所以它就有了CObject 所支持的一切性质，包括执行时期型别信息（RTTI）、动态生成（Dynamic Creation）、文件读写（Serialization）。   
又由于它也衍生自CCmdTarget，所以它可以接收来自菜单或工具栏的WM_COMMAND消息。   
### View
View负责显示Document中的数据。   
如Document一样，我们也要从CView中派生一个自己的类出来，并且改写其中负责显示资料的OnDraw函数或者OnPrint函数。前者负责在显示屏上显示，后者用于打印机。   
```c 
class CScribbleView : public CView   
{   
	DECLARE_DYNCREATE(CScribbleView)  
	//...  
	virtual void OnDraw(CDC* pDC);   
	DECLARE_MESSAGE_MAP()   
};   
void CScribbleView::OnDraw(CDC* pDC)  
{   
	CScribbleDoc* pDoc = GetDocument();  
	ASSERT_VALID(pDoc);   
	// TODO: add draw code for native data here   
}  
```    
由于CView 衍生自CWnd，所以它可以接收一般Windows 消息（如WM_SIZE、WM_PAINT 等等），又由于它也衍生自CCmdTarget，所以它可以接收来自菜单或工具列的WM_COMMAND 消息。   
View 事实上是个没有边框的窗口。真正出现时，其外围还有一个有边框的窗口，我们称为Frame窗口。    
### Document Frame
不同的文件类型例如TEXT和BITMAP需要不同的UI，而UI部分是由Frame负责的。而这些内容是被预设好的，并且可以被修改。   
### Document Template
每当使用者使用MFC软件打开一份文件，程序都应该生成Document,view,Frame各一份。这三个部分成为一个运作单元，由DOcument Template管理。即MFC中的CDocTemplate类。   
同时它又有两个派生类别，即CmultiDocTemplate和CSingleDocTemplate类。这两个类前者可以处理多种类型的文件，后者只能处理单一类型的文件，因此除非我们确认该软件只会用到一种资料，不然我们还是选前者比较好。  
需要注意的是，这与MDI和SDI是没有关系的，MDI和SDI指的是同时只能打开一份或可以打开多份资料。注意是打开，而不是可处理。   
CDocTemplate 是个抽象类别，定义了一些用来处理「Document/View/Frame 三口组」的基础函数。   
### CDocTemplate管理CDocument/CView/CframeWnd
Document Template管理Document,CView,CframeWNd，同时它被CwinApp管理。   
文件是如何开启的？  
```c
BEGIN_MESSAGE_MAP(CScribbleApp, CWinApp)   
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)   
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)  
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)    
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)    
END_MESSAGE_MAP()    
```   
通过消息映射，由CWinApp::OnFileNew和CWinApp::OnFileOpen开启。   
但是并不是由CWinapp生成Document,Frame和View的它只负责选择适当的DOcument Template，然后由Document Template来继续后面的事情。   
如图所示：   
![](https://github.com/sii2017/image/blob/master/Document%20Template.png)   
CDocTemplate CDocument CView CFrameWnd的关系：  
![](https://github.com/sii2017/image/blob/master/CDocTemplate%20CDocument%20CView%20CFrameWnd%E7%9A%84%E5%85%B3%E7%B3%BB.png)   
### CObject类别
绝大部分的MFC类别，以及我们自己的类别都需要从CObject派生的原因是，我们可以继承它许多重要的特性。   
CObject这个基类至少提供了两个重要机能及虚拟函数：IsKindOf和IsSerializable。    
#### IsKindOf
IsKindOf是RTTI的化身，之前已经简单说过MFC的RTTI了。通过这个函数可以在运行时了解，某个类型的指针由于多态，指向的究竟是哪个类型。   
下面是CObject::IsKindOf 虚拟函数的源代码：  
```c
// in OBJCORE.CPP
BOOL CObject::IsKindOf(const CRuntimeClass* pClass) const   
{   
	// simple SI case   
	CRuntimeClass* pClassThis = GetRuntimeClass();  
	return pClassThis->IsDerivedFrom(pClass);     
}      

BOOL CRuntimeClass::IsDerivedFrom(const CRuntimeClass* pBaseClass) const    
{  
	// simple SI case   
	const CRuntimeClass* pClassThis = this;   
	while (pClassThis != NULL)  
	{   
		if (pClassThis == pBaseClass)  
		return TRUE;    
		pClassThis = pClassThis->m_pBaseClass;   
	}    
	return FALSE; // walked to the top, no match  
}   
```   
#### IsSerializable
一个类别若要能够进行Serialization 动作，必须准备Serialize 函数，并且在「类别型录网」中自己的那个CRuntimeClass 元素里的schema 字段里设立0xFFFF 以外的号码，代表资料格式的版本（这样才能提供机会让设计较佳的Serialize 函数能够区分旧版资料或新版资料， 避免牛头不对马嘴的困惑） 。这些都是DECLARE_SERIAL 和IMPLEMENT_SERIAL 宏的责任范围。    
CObject 提供了一个虚拟函数，让程序在执行时期判断某类别的schema 号码是否为0xFFFF，藉此得知它是否可以Serialize：   
```c
BOOL CObject::IsSerializable() const   
{   
	return (GetRuntimeClass()->m_wSchema != 0xffff);   
}   
```   
### CArchive 类别
在之前使用Serialize函数的时候可以看到，在函数内我们并不直接操作内存或者硬盘上的文件，而是对CArchive的对象进行操作。这是因为，我们会把文件放在CArchive对象中，相当于一个内存缓冲区。   
以下源代码：  
```c
BOOL CDocument::OnSaveDocument(LPCTSTR lpszPathName)  
{   
	CFile* pFile = NULL;   
	pFile = GetFile(lpszPathName, CFile::modeCreate |
	CFile::modeReadWrite | CFile::shareExclusive, &fe);   
	// 令file 和archive 产生关联    
	CArchive saveArchive(pFile, CArchive::store |
	CArchive::bNoFlushOnDelete);   
	...   
	Serialize(saveArchive);  
	...   
	saveArchive.Close();   
	ReleaseFile(pFile, FALSE);   
}    
BOOL CDocument::OnOpenDocument(LPCTSTR lpszPathName)      
{   
	CFile* pFile = GetFile(lpszPathName,
	CFile::modeRead|CFile::shareDenyWrite, &fe);      
	//令file 和archive 产生关联   
	CArchive loadArchive(pFile, CArchive::load |
	　　 CArchive::bNoFlushOnDelete);    
	...   
	Serialize(loadArchive);   
	...   
	loadArchive.Close();  
	ReleaseFile(pFile, FALSE);   
}   
```   