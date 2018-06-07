## MFC程序的生死因果
以传统的C/SDK撰写Windows 程序，最大的好处是可以清楚看见整个程序的来龙去脉和消息动向，然而这些重要的动线在MFC应用程序中却隐晦不明，因为它们被Application Framework封装起来了。   
本章主要目的除了解释MFC应用程序的长像，也要从MFC源代码中检验出一个Windows程序原本该有的程序进入点（WinMain）、视窗类别注册（RegisterClass）、窗口产生（CreateWindow）、消息循环（Message Loop）、窗口函数（Window Procedure）等等动作，抽丝剥茧彻底了解一个MFC 程序的诞生与结束，以及生命过程。   
> 也许使用MFC不需要了解其中原理，但是了解了其中原理可以更好的操控MFC。   
### 熟记MFC 类别的阶层架构
   
