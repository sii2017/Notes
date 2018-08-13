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