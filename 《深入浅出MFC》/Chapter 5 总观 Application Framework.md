## 总观 Application Framework
Application Framework是时代的潮流，它将逐渐的取代原有的从零造起，而使用一整套模板来加以改造的方式。   
### Afx全局函数
MFC之中有存在有不属于任何类别的全域函数，它们统统在函数名称开头冠以Afx。   
下面是几个常见的Afx 全域函数：   
AfxWinInit 被WinMain（由MFC 提供）调用的一个函数，用做MFC GUI
程序初始化的一部份，请看第６章的「AfxWinInit-AFX内部初始化动作」一节。如果你写一个MFC console 程序，就得自行调用此函数（请参考Visual C++ 所附之Tear 范例程序）。   
AfxBeginThread 开始一个新的执行线程（请看第14 章，# 756 页）。   
AfxEndThread 结束一个旧的执行线程（请看第14 章，# 756 页）。   
AfxFormatString1 类似printf 一般地将字符串格式化。   
AfxFormatString2 类似printf 一般地将字符串格式化。   
AfxMessageBox 类似Windows API 函数MessageBox。    
AfxOutputDebugString 将字符串输往除错装置（请参考附录D，# 924 页）。   
AfxGetApp 取得application object（CWinApp 衍生对象）的指针。   
AfxGetMainWnd 取得程序主窗口的指针。   
AfxGetInstance 取得程序的instance handle。    
AfxRegisterClass 以自定的WNDCLASS 注册窗口类别（如果MFC 提供的数个窗口类别不能满足你的话）。    
### MFC宏（macros）
CObject 和CRuntimeClass 之中封装了数个所谓的object services，包括「取得执行时期的类别信息」（RTTI）、Serialization（文件读写）、动态产生对象...等等。所有衍生自CObject的类别，都继承这些机能。  
■ 取得执行时期的类别信息（RTTI），使你能够决定一个执行时期的对象的类别信息，这样的能力在你需要对函数参数做一些额外的类型检验，或是当你要针对对象属于某种类别而做特别的动作时，份外有用。    
■ Serialization 是指将对象内容写到文件中，或从文件中读出。如此一来对象的生命就可以在程序结束之后还延续下去，而在程序重新激活之后，再被读入。这样的对象可说是"persistent"（永续存在）。    
■ 所谓动态的对象生成（Dynamic object creation），使你得以在执行时期产生一个特定的对象。例如document、view、和frame 对象就都必须支持动态对象生成，因为framework 需要在执行时期产生它们（第８章有更详细的说明）。    
**MFC 针对上述这些机能，准备了一些宏，让程序能够很方便地继承并实作出上述四大机能。这些宏包括：**   
DECLARE\_DYNAMIC 执行时期类别信息 第３章、第８章  
IMPLEMENT\_DYNAMIC 执行时期类别信息 第３章、第８章    
DECLARE\_DYNCREATE 动态生成 第３章、第８章   
IMPLEMENT\_DYNCREATE 动态生成 第３章、第８章   
DECLARE\_SERIAL 对象内容的文件读写 第３章、第８章   
IMPLEMENT\_SERIAL 对象内容的文件读写 第３章、第８章   
DECLARE\_OLECREATE OLE 对象的动态生成 不在本书范围之内   
IMPLEMENT\_OLECREATE OLE 对象的动态生成 不在本书范围之内      
**在第３章提过MFC 的消息映射（Message Mapping）与命令绕（Command Routing）两个特性。这两个性质系由以下这些MFC 宏完成：**   
DECLARE\_MESSAGE\_MAP 声明消息映射表数据结构 第３章、第９章   
BEGIN\_MESSAGE_MAP 开始消息映射表的建置 第３章、第９章   
ON\_COMMAND 增加消息映射表中的项目 第３章、第９章   
ON\_CONTROL 增加消息映射表中的项目 本书未举例    
ON\_MESSAGE 增加消息映射表中的项目 本书未举例   
ON\_OLECMD 增加消息映射表中的项目 本书未举例   
ON\_REGISTERED\_MESSAGE 增加消息映射表中的项目 本书未举例    
ON\_REGISTERED\_THREAD\_MESSAGE 增加消息映射表中的项目 本书未举例   
ON\_THREAD\_MESSAGE 增加消息映射表中的项目 本书未举例  
ON\_UPDATE\_COMMAND\_UI 增加消息映射表中的项目 第３章、第９章    
END\_MESSAGE\_MAP 结束消息映射表的建置 第３章、第９章    
> 事实上，与其它MFC Programming 书籍相比较，本书最大的一个特色就是，要把上述这些MFC宏的来龙去脉交待得非常清楚。我认为这对于撰写MFC 程序是非常重要的一件事。    