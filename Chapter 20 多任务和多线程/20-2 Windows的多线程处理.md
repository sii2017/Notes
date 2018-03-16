## Windows的多线程处理
建立新的线程的API函数是CreateThread，它的语法如下：   
```c
hThread= CreateThread (&security_attributes, dwStackSize, ThreadProc, pParam, dwFlags, &idThread);   
```   
第一个参数是指向SECURITY_ATTRIBUTES型态的结构的指针。在Windows 98中忽略该参数。在Windows NT中，它被设为NULL。    
第二个参数是用于新线程的初始堆栈大小，默认值为0。**在任何情况下，Windows根据需要动态延长堆栈的大小**。   
第三个参数是指向线程函数的指标。函数名称没有限制，但是必须以下列形式声明：    
```c
DWORD WINAPI ThreadProc (PVOID pParam);   
```   
第四个参数为传递给ThreadProc的参数。这样主线程和从属线程就可以共享数据。   
第五个参数通常为0，但当建立的线程不马上执行时为旗标CREATE_SUSPENDED。线程将暂停直到呼叫ResumeThread来恢复线程的执行为止。    
第六个参数是一个指针，指向接受执行绪ID值的变量。   
大多数Windows程序写作者喜欢用在PROCESS.H表头文件中声明的C执行时期链接库函数_beginthread来替代CreateThread。它的语法如下：   
```c
hThread = _beginthread (ThreadProc, uiStackSize, pParam) ;   
```    
它更简单，对于大多数应用程序很完美，这个线程函数的语法为：   
```c    
void __cdecl ThreadProc (void * pParam) ;   
```   
### 再论随机矩形
RNDRCTMT是第五章里的RANDRECT程序的多线程版本。   
参考RNDRCTMT.C   
窗口消息处理程序以最容易的方法调用_beginthread－简单地以线程函数的地址（称为Thread）作为第一个参数，其它参数使用0，线程函数传回VOID并有一个参数，该参数是一个指向VOID的指标。在RNDRCTMT中的Thread函数不使用这个参数。  
在调用了_beginthread函数之后，线程函数（以及该线程函数可能呼叫的其它任何函数）中的程序代码和程序中的其它程序代码**同时执行**。两个或者多个执行绪使用一个程序中的同一函数，在这种情况下，动态区域变量（储存在堆栈上）对每个执行绪是唯一的。对程序中的所有执行绪来说，所有的静态变量都是一样的。因此设定cxClient和cyClient为全局变量来给线程函数使用。    
有时您需要唯一于各个线程的持续储存性数据。通常，这种数据是静态变量，但在Windows 98中，可以使用「线程区域储存空间」，我将在本章后面进行讨论。     
### 程序设计竞赛的问题
参考MULTI1.C
### 多线程解决方案
参考MULTI2.C
### 有问题嘛？
### Sleep的好处