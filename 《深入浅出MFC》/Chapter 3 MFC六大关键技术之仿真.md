## MFC六大关键技术之仿真
以仿真为基础，增进了解application framework的内部运作和设计方式。  
整个MFC 4.0 多达189 个类别，源代码达252 个实作档，58 个头文件，共10 MB 之多。MFC 4.2 又多加了29 个类别。   
本章挑选最重要的，与应用程序主干息息相关的6个题目，包括：  
■ MFC程序的初始化过程   
■ RTTI（Runtime Type Information）执行时期型别信息   
■ Dynamic Creation 动态生成   
■ Persistence 永续留存  
■ Message Mapping 消息映射   
■ Message Routing 消息绕行   
本章的一些函数和概念皆为“仿真”，并非MFC的实际面貌，只是非常接近。并且将代码简化再简化，简化到不能再简化。   
### MFC程序的初始化过程
先构建一个极简的，但是类似MFC的阶层关系。   
![](https://github.com/sii2017/image/blob/master/MFC%E6%9E%81%E7%AE%80%E7%BB%93%E6%9E%84.jpg)    
### MFC程序的初始化过程
这块建议看书了解，比较复杂，难以说明清楚。   
在MFC中会先有一个全局变量  
```c
CMyWinApp theApp;  
```  
全局变量先于main创建（即构造），随后在main函数中会有一个指针指向它：   
```c
CWinApp* pApp = AfxGetApp();//AfxGetApp()返回的就是theApp      
```   
随后分别有两个MFC函数进行调用，这里就设计到虚函数的概念，具体看不同类有没有重写，来导致具体调用的是哪个类的函数：   
```c   
pApp->InitApplication();//整个程序只调用一次的初始化    
pApp->InitInstance(); //每个实例都会调用一次的初始化   
pApp->Run();     
```    
随着继承以及虚函数的使用，几个简单的调用就蝴蝶效应般创建了大部分（也可能是全部）的不同的类，尽管我们没有使用new。   
### RTTI（执行时期型别辨识）   
我们将仿制出来RTTI的能力，即使我们的类具有IsKindOf的能力，能在执行时侦察某个对象是否属于某种类别并传回true或false。    
例如以MFC 的类别阶层来说，我们希望：   
```c
CMyDoc* pMyDoc = new CMyDoc;   
cout << pMyDoc->IsKindOf(CMyDoc); // 应该获得1（TRUE）   
cout << pMyDoc->IsKindOf(CDocument); // 应该获得1（TRUE）  
cout << pMyDoc->IsKindOf(CCmdTarget); // 应该获得1（TRUE）   
cout << pMyDoc->IsKindOf(CWnd); // 应该获得0（FALSE）   
```    
我们将建立一个“类别型录”来记录类的各种信息以供查阅，以CRuntimeClass描述之，那是一个结构，内中至少需有类别名称、串行的Next 指针，以及串行的First指针。其中First指针只需要一份，以static修饰之。       
```c
struct CRuntimeClass   
{  
	LPCSTR m_lpszClassName;//类名   
	int m_nObjectSize;   
	UINT m_wSchema; // schema number of the loaded class   
	CObject* (PASCAL* m_pfnCreateObject)(); // NULL => abstract class   
	CRuntimeClass* m_pBaseClass; //基类/父类   
	// CRuntimeClass objects linked together in simple list    
	static CRuntimeClass* pFirstClass; //类似于链表，第一个为静态指针       
	CRuntimeClass* m_pNextClass; //下一个类，并非子类     
};   
```   
在经由某种手段记录后，类别型录将呈现以下的样貌：   
![](https://github.com/sii2017/image/blob/master/%E7%B1%BB%E5%88%AB%E5%9E%8B%E5%BD%95.jpg)   
但是如何将类名添加进去呢？   
### DECLARE\_DYNAMIC / IMPLEMENT\_DYNAMIC宏    
为了神不知鬼不觉把CRuntimeClass对象塞到类别之中，并声明一个可以抓到该对象地址的函数，我们定义DECLARE_DYNAMIC宏如下：   
```c
#define DECLARE_DYNAMIC(class_name) \   
public: \   
static CRuntimeClass class##class_name; \  
virtual CRuntimeClass* GetRuntimeClass() const;   
```   
出现在宏定义之中的##，用来告诉编译器，把两个字符串系在一起。如果你这么使用此宏：   
```c
DECLARE_DYNAMIC(CView)   
```
编译器前置处理器为你做出的码是：    
```c
public:   
	static CRuntimeClass classCView;//注意，这里变成了classCView   
	virtual CRuntimeClass* GetRuntimeClass() const;//抓到该对象地址的虚函数    
```
如此，只要在声明类别时放入DECLARE_DYNAMIC宏就行了。   
另外，类别型录（也就是各个CRuntimeClass对象）的内容指定以及串接工作最好也能够神不知鬼不觉，我们于是再定义IMPLEMENT_DYNAMIC宏：    
```c
#define IMPLEMENT_DYNAMIC(class_name, base_class_name) \    
_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0xFFFF, NULL)    
```    
其中的\_IMPLEMENT\_RUNTIMECLASS 又是另一个宏：   
```c
#define _IMPLEMENT_RUNTIMECLASS(class_name, base_class_name,wSchema,pfnNew) \   
static char _lpsz##class_name[] = #class_name; \  
CRuntimeClass class_name::class##class_name = { \    
_lpsz##class_name, sizeof(class_name), wSchema, pfnNew, \   
RUNTIME_CLASS(base_class_name), NULL }; \   
static AFX_CLASSINIT _init_##class_name(&class_name::class##class_name); \   
CRuntimeClass* class_name::GetRuntimeClass() const \   
{ return &class_name::class##class_name; } \    
```   
其中又有RUNTIME_CLASS 宏，定义如下：   
```c
#define RUNTIME_CLASS(class_name) \   
(&class_name::class##class_name)   
```   
其中，还使用了一个struct AFX_CLASSINIT，定义如下：   
```c
struct AFX_CLASSINIT
{ AFX_CLASSINIT(CRuntimeClass* pNewClass); };//这是个构造函数   
```   
该构造函数如下：   
```c
AFX_CLASSINIT::AFX_CLASSINIT(CRuntimeClass* pNewClass)   
{    
	pNewClass->m_pNextClass = CRuntimeClass::pFirstClass;   
	CRuntimeClass::pFirstClass = pNewClass;   
}    
```     
该构造函数负责将类别通过链表的方式连接起来，新的类成为FirstClass，而之前的Firstclass成为NextClass。   
于是，程序中只需要简简单单的两个宏DECLARE\_DYNAMIC(Cxxx)和IMPLEMENT\_DYNAMIC(Cxxx, Cxxxbase) ，就完成了构造资料并加入串行的工作。     
但是，串行的头需要特别处理，不能够套用一般的串行行为模式。我们的类别根源CObject，不能套用现成的宏DECLARE\_DYNAMIC和IMPLEMENT\_DYNAMIC，必须特别设计如下：   
```c 
// in header file
class CObject
{
	public:
	virtual CRuntimeClass* GetRuntimeClass() const;
	...
	public:
	static CRuntimeClass classCObject;
};
// in implementation file
static char szCObject[] = "CObject";
struct CRuntimeClass CObject::classCObject =
{ szCObject, sizeof(CObject), 0xffff, NULL, NULL };
static AFX_CLASSINIT _init_CObject(&CObject::classCObject);
CRuntimeClass* CObject::GetRuntimeClass() const
{
	return &CObject::classCObject;
}
```   
### 型别辨识IsKindOf
有了上面的类别型录，就可以比较轻松的实现IsKindOf功能了。    
为CObject 加上一个IsKindOf 函数，于是此函数将被所有类别继承。它将把参数所指定的某个CRuntimeClass 对象拿来与类别型录中的元素一一比对。比对成功（在型录中有发现），就传回TRUE，否则传回FALSE：   
```c
// in header file  
class CObject  
{  
	public:  
	//...    
	BOOL IsKindOf(const CRuntimeClass* pClass) const;   
};   
// in implementation file    
BOOL CObject::IsKindOf(const CRuntimeClass* pClass) const  
{   
	CRuntimeClass* pClassThis = GetRuntimeClass(); //获取当前函数的指针  
	while (pClassThis != NULL)  
	{  
		if (pClassThis == pClass)   
			return TRUE;   
		pClassThis = pClassThis->m_pBaseClass;//注意这里比较的是父类   
	}   
	return FALSE; // walked to the top, no match   
}   
```    
### Dynamic Creation (动态生成）
如果我能够把类别的大小记录在类别型录中，把构造函数（注意，这里并非指C++ 构造式，而是指即将出现的CRuntimeClass::CreateObject）也记录在类别型录中，当程序在执行时期获得一个类别名称，它就可以在「类别型录网」中找出对应的元素，然后调用其构造函数（这里并非指C++ 构造式），产生出对象。    
因此原来的类别型录网的CRuntimeClass于是有了变化：   
```c
struct CRuntimeClass  
{   
	LPCSTR m_lpszClassName;  
	int m_nObjectSize;   
	UINT m_wSchema; // schema number of the loaded class   
	CObject* (PASCAL* m_pfnCreateObject)(); // NULL => abstract class    
	CRuntimeClass* m_pBaseClass;  

	//新加入的两行代码   
	CObject* CreateObject();   
	static CRuntimeClass* PASCAL Load();   

	// CRuntimeClass objects linked together in simple list    
	static CRuntimeClass* pFirstClass; // start of class list    
	CRuntimeClass* m_pNextClass; // linked list of registered classes    
};
```    
### DECLARE\_DYNCREATE/IMPLEMENT\_DYNCREATE宏  
这两个宏区别于之前的两个宏，这两个新的宏加入了之前加入的新的两个函数。   
```c
#define DECLARE_DYNCREATE(class_name) \   
DECLARE_DYNAMIC(class_name) \  
static CObject* PASCAL CreateObject();   
#define IMPLEMENT_DYNCREATE(class_name, base_class_name) \   
CObject* PASCAL class_name::CreateObject() \  
{ return new class_name; } \    
_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0xFFFF, \   
class_name::CreateObject)   
```    
从宏的定义我们很清楚可以看出，拥有动态生成（Dynamic Creation）能力的类别库，必然亦拥有执行时期类型识别（RTTI）能力，因为\_DYNCREATE 宏涵盖了\_DYNAMIC宏。   
### Persistence（永续生存）机制
MFC 有一套Serialize 机制，目的在于把档名的选择、文件的开关、缓冲区的建立、资料的读写、萃取运算子（>>）和嵌入运算子（<<）的重载（overload）、对象的动态生成...都包装起来。   
我们可以在每次记录对象内容的时候，先写入一个代码，表示此对象之类别是否曾在档案中记录过了。如果是新类别，乖乖地记录其类别名称；如果是旧类别，则以代码表示。这样可以节省文件大小以及程序用于解析的时间。    
![](https://github.com/sii2017/image/blob/master/Serialize.jpg)    
每一个可写到文件或可从文件中读出的类别，都应该有它自己的Serailize 函数（虚函数重写），负责它自己的资料读写文件动作。此类别并且应该改写<< 运算子和>> 运算子，把资料导流到archive 中。archive是什么？是一个与文件息息相关的缓冲区，暂时你可以想象它就是文件的化身。  
### DECLARE\_SERIAL / IMPLEMENT\_SERIAL 宏
类别之能够进行文件读写动作，前提是拥有动态生成的能力，所以，MFC 设计了两个宏DECLARE\_SERIAL 和IMPLEMENT\_SERIAL：   
```c
#define DECLARE_SERIAL(class_name) \   
DECLARE_DYNCREATE(class_name) \   
//新加入的友元    
friend CArchive& AFXAPI operator>>(CArchive& ar, class_name* &pOb);   
#define IMPLEMENT_SERIAL(class_name, base_class_name, wSchema) \   
CObject* PASCAL class_name::CreateObject() \   
{ return new class_name; } \   
_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, \   
class_name::CreateObject) \   
//新加入的符号重载    
CArchive& AFXAPI operator>>(CArchive& ar, class_name* &pOb) \   
{ pOb = (class_name*) ar.ReadObject(RUNTIME_CLASS(class_name)); \   
return ar; } \    
```   
为了在每一个对象被处理（读或写）之前，能够处理琐屑的工作，诸如判断是否第一次出现、记录版本号码、记录文件名等工作，CRuntimeClass需要两个函数Load和Store:      
```c
struct CRuntimeClass   
{    
	LPCSTR m_lpszClassName;     
	int m_nObjectSize;     
	UINT m_wSchema; // schema number of the loaded class   
	CObject* (PASCAL* m_pfnCreateObject)(); // NULL => abstract class   
	CRuntimeClass* m_pBaseClass;    

	CObject* CreateObject();    
	//两行新加入的代码   
	void Store(CArchive& ar) const;   
	static CRuntimeClass* PASCAL Load(CArchive& ar, UINT* pwSchemaNum);    

	// CRuntimeClass objects linked together in simple list   
	static CRuntimeClass* pFirstClass; // start of class list    
	CRuntimeClass* m_pNextClass; // linked list of registered classes     
};
```
store和load函数的定义如下：  
```c
CRuntimeClass* PASCAL CRuntimeClass::Load(CArchive& ar, UINT* pwSchemaNum)   
{  
	WORD nLen;   
	char szClassName[64];   
	CRuntimeClass* pClass;   
	ar >> (WORD&)(*pwSchemaNum) >> nLen;     
	if (nLen >= sizeof(szClassName) || ar.Read(szClassName, nLen) != nLen)   
		return NULL;  
	szClassName[nLen] = '\0';   
	for (pClass = pFirstClass; pClass != NULL; pClass = pClass->m_pNextClass)   
	{   
		if (lstrcmp(szClassName, pClass->m_lpszClassName) == 0)   
		return pClass;  
	}    
	return NULL; // not found   
}     
void CRuntimeClass::Store(CArchive& ar) const   
// stores a runtime class description  
{    
	WORD nLen = (WORD)lstrlenA(m_lpszClassName);  
	ar << (WORD)m_wSchema << nLen;   
	ar.Write(m_lpszClassName, nLen*sizeof(char));   
}   
```   
### Message Mapping（消息映射）
一般来说，我们的类别库成立，我们将会设计，让消息从子类，慢慢向上传递，经由中间类，最后到基类，使消息以单鞭的形式经过所有的类别。   
然而MFC之中用来处理消息的C++类别，并不呈单鞭发展。作为application framework的重要架构之一的document/view，也具有处理消息的能力（你现在可能还不清楚什么是document/view，没有关系）。因此，消息藉以攀爬的路线应该有横流的机会：    
![](https://github.com/sii2017/image/blob/master/%E6%B6%88%E6%81%AF%E6%B5%81%E5%8A%A8.jpg)    
在这里，消息如何流动，我们暂时先不管。是直线前进，或是中途换跑道，我们都暂时不管，本节先把这个攀爬路线网建立起来再说。   
这整个攀爬路线网就是所谓的消息映射表（Message Map）。将消息与表格中的元素比对，然后调用对应的处理代码，这种动作我们也称之为消息映射（Message Mapping）。    
为了尽量降低对正常（一般）类别声明和定义的影响，我们希望，最好能够像RTTI和Dynamic Creation一样，用一两个宏就完成这巨大蜘蛛网的构造。最好能够像DECLARE\_DYNAMIC和IMPLEMENT\_DYNAMIC宏那么方便。   
首先定义一个数据结构：   
```c
struct AFX_MSGMAP   
{   
	AFX_MSGMAP* pBaseMessageMap;   
	AFX_MSGMAP_ENTRY* lpEntries;   
};   
//其中的AFX_MSGMAP_ENTRY又是另一个数据结构：    
struct AFX_MSGMAP_ENTRY // MFC 4.0 format   
{   
	UINT nMessage; // windows message    
	UINT nCode; // control code or WM_NOTIFY code   
	UINT nID; // control ID (or 0 for windows messages)    
	UINT nLastID; // used for entries specifying a range of control id's    
	UINT nSig; // signature type (action) or pointer to message #      
	AFX_PMSG pfn; //将要调用的函数指针
};    
//其中的AFX_PMSG 定义为函数指针：   
typedef void (CCmdTarget::*AFX_PMSG)(void);    
```
然后就是定义一个与DECLARE\_DYNAMIC和IMPLEMENT\_DYNAMIC类似的宏了：   
```c
#define DECLARE_MESSAGE_MAP() \   
static AFX_MSGMAP_ENTRY _messageEntries[]; \   
static AFX_MSGMAP messageMap; \   
virtual AFX_MSGMAP* GetMessageMap() const;   
```   
这个数据结构的内容填塞工作由三个宏完成：   
```c
#define BEGIN_MESSAGE_MAP(theClass, baseClass) \   
AFX_MSGMAP* theClass::GetMessageMap() const \  
{ return &theClass::messageMap; } \   
AFX_MSGMAP theClass::messageMap = \   
{ &(baseClass::messageMap), \   
(AFX_MSGMAP_ENTRY*) &(theClass::_messageEntries) }; \     
AFX_MSGMAP_ENTRY theClass::_messageEntries[] = \   
{   
#define ON_COMMAND(id, memberFxn) \   
{ WM_COMMAND, 0, (WORD)id, (WORD)id, AfxSig_vv, (AFX_PMSG)memberFxn },   
#define END_MESSAGE_MAP() \   
{ 0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 } \     
};    
```
**以CView 为例，下面的代码：**   
```c
// in header file  
class CView : public CWnd   
{   
public:    
	//...   
	DECLARE_MESSAGE_MAP()   
};  
// in implementation file   
#define CViewid 122  
...   
BEGIN_MESSAGE_MAP(CView, CWnd)    
ON_COMMAND(CViewid, 0)   
END_MESSAGE_MAP()       
```   
展开后则是：   
```c
// in header file   
class CView : public CWnd   
{   
public:   
	//...   
	static AFX_MSGMAP_ENTRY _messageEntries[];    
	static AFX_MSGMAP messageMap;   
	virtual AFX_MSGMAP* GetMessageMap() const;   
};   
// in implementation file    
#define CViewid 122  //定义了一下号码，在_messageEntries中使用     
AFX_MSGMAP* CView::GetMessageMap() const  
{ return &CView::messageMap; }   
AFX_MSGMAP CView::messageMap =   
{ &(CWnd::messageMap),
(AFX_MSGMAP_ENTRY*) &(CView::_messageEntries) };//指针指向基类的messageMap     
AFX_MSGMAP_ENTRY CView::_messageEntries[] =   
{    
	{ WM_COMMAND, 0, (WORD)122, (WORD)122, 1, (AFX_PMSG)0 },    
	{ 0, 0, 0, 0, 0, (AFX_PMSG)0 }   
};    
```    
以图表示则为：    
![](https://github.com/sii2017/image/blob/master/%E6%B6%88%E6%81%AF%E6%98%A0%E5%B0%84%E4%BB%BF%E7%9C%9F.png)   