## MFC多线程程序设计
模块：一段可执行的程序（包括exe和dll），其程序代码、资料、资源被加载到内存中，由系统建立一个数据结构来管理他，这就是一个模块。   
进程：进程是一大堆拥有权（权限）的集合。进程拥有地址空间、动态配置而来的内存、文件、执行线程和一系列的模块。   
线程：进程主要表达拥有权，执行线程则主要表达模块中的程序代码的执行事实。   
### 与执行线程有关的Win32API
![](https://github.com/sii2017/image/blob/master/%E4%B8%8E%E6%89%A7%E8%A1%8C%E7%BA%BF%E7%A8%8B%E6%9C%89%E5%85%B3%E7%9A%84api.png)  
在MFC中的大部分与线程有关的函数，都是封装了以上win32的api的。   
### Worker Threads和UI Threads
在MFC中，把线程分成了worker thread和UI thread。实际上我们一般使用的线程就是worker thread，通过函数来调用，使用完毕则结束。UI thread可以看作是一个长期存在的线程，比如我们将按钮框架等等以另外的线程的方式来创建，并且他需要一直存在直到结束，以及它会接受相关的消息映射。  
尽管我并不觉得这是个好主意。  
作者也觉得这不是个好主意。   
### MFC的多线程设计
MFC的线程，与其它的类似，也是通过一个类来动态生成的。   
CwinThread类。   
虽然MFC 程序只会有一个CWinApp 对象，而CWinApp 衍生自CWinThread，但并不是说一个MFC 程序只能有一个CWinThread 对象。每当你需要一个额外的执行线程，不应该在MFC 程序中直接调用::CreateThread 或_beginthreadex，应该先产生一个CWinThread 对象，再调用其成员函数CreateThread 或全域函数AfxBeginThread 将执行线程产生出来。   
### worker thread和 UI thread不同的调用方法
由于UI thread几乎不常用，因此不详述，略。   
worker thread的用法与一般的产生线程的方式相似。   
### 同步控制
MFC提供了四个对应的类别以达到多线程之间的不同控制。   
![](https://github.com/sii2017/image/blob/master/MFC%E5%AF%B9%E7%BA%BF%E7%A8%8B%E7%9A%84%E5%9B%9B%E7%A7%8D%E6%8E%A7%E5%88%B6.png)   
因为内容牵扯太多，作者略了。  
那我也略了。  
如果之后接触到的话，会在blog中进行相关的更新。   