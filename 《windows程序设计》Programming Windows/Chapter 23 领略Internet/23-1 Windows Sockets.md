# 第二十章 领略Internet
Internet－全世界计算机透过不同协议交换信息的大型连结体－近几年重新定义了个人计算的几个领域。虽然拨接信息服务和电子邮件系统在Internet流行开来之前就已经存在，但它们通常局限于文字模式，并且根本没有连结而是各自分隔的。例如，每一种信息服务都需要拨不同的电话号码，用不同的使用者ID和密码登录。每一种电子邮件系统仅允许在特定系统的缴款使用者之间发送和接收邮件。      
现在，往往只需要拨单一支电话就可以连结整个Internet，而且可以和有电子邮件地址的人进行全球通信。特别是在World Wide Web上，超文字、图形和多媒体（包括声音、音乐和视讯）的使用已经扩展了在线信息的范围和功能。     
如果要提供涵盖Windows中所有与Internet相关程序设计问题的彻底介绍，可能还需要再加上几本书才够。所以，本章实际上主要集中在如何让小型的Microsoft Windows应用程序能够有效地从Internet上取得信息的两个领域。**这两个领域分别是Windows Sockets (Winsock) API和Windows Internet（WinInet）API支持的文件传输协议（FTP：File Transfer Protocol）的部分**。     
## Windows Sockets
Socket是由University of California在Berkeley分校开发的概念，用于在UNIX操作系统上添加网络通讯支持。那里开发的API现在称为「Berkeley socket interface」
### Sockets和TCP/IP
Socket通常（但不专用于）与主宰Internet通信的传输控件协议/因特网协议（TCP/IP：Transmission Control Protocol/Internet Protocol）牵连在一起。因特网协定（IP：Internet Protocol），作为TCP/IP的组成部分之一，用来将数据打包成「数据封包（datagram）」，该资料封包包含用于标识数据来源和目的地的表头信息。而传输控制协议（TCP：Transmission Control Protocol）则提供了可靠的传输和检查IP数据封包正确性的方法。    
在TCP/IP下，通讯端点由IP地址和端口号定义。IP地址包括4个字节，用于确定Internet上的服务器。IP地址通常按「由点连结的四个小于255的数字」的格式显示，例如「209.86.105.231」。埠号确定了特定的服务或服务器提供的服务。其中一些埠号已经标准化，以提供众所周知的服务。    
**当Socket与TCP/IP合用时，Socket就是TCP/IP的通讯端点。因此，Socket指定了IP地址和端口号。**    
### 网络时间服务
下面给出的范例程序与提供时间协议（Time Protocol）的Internet服务器相连结。此程序将获得目前准确的日期和时间，并用此信息设定您的PC时钟。   
在美国，国家标准和技术协会（National Institute of Standards and Technology）（以前称为国家标准局（National Bureau of Standards））负责维护准确时间，该时间与世界各地的机构相联系。准确时间可用于无线电广播、电话号码、计算机拨号电话号码以及Internet，关于这些的所有文件都位于网站http://www.bldrdoc.gov/timefreq（网域名称「bldrdoc」指的是Boulder、Colorado、NIST Time的位置和Frequency Division）       
**我们只对NIST Network Time Service感兴趣，其详细的文件位于http://www.bldrdoc.gov/timefreq/service/nts.htm。此网页列出了十个提供NIST时间服务的服务器。例如，第一个名称为time-a.timefreq.bldrdoc.gov，其IP地址为132.163.135.130。**    
在Internet上有三个不同的时间服务，每一个都由Request for Comment（RFC）描述为Internet标准。    
**日期协议**（Daytime Protocol）（RFC-867）提供了一个ASCII字符串用于指出准确的日期和时间。该ASCII字符串的准确格式并不标准，但人们可以理解其中的含义。    
**时间协议**（RFC-868）提供了一个32位的数字，用来表示从1900年1月1日至今的秒数。该时间是UTC（不考虑字母顺序，它表示世界时间坐标（Coordinated Universal Time）），它类似于所谓的格林威治标准时间（Greenwich Mean Time）或者GMT－英国格林威治时间。   
第三个协议称为**网络时间协议**（Network Time Protocol）（RFC-1305），该协议很复杂。     
对于我们的目的，即包括分析Socket和不断更新PC时钟，时间协议RFC-868已经够用了。RFC-868只是一个两页的简短文件，主要是说用TCP获得准确时间的程序应该有如下步骤：       
> 连结到提供此服务的服务器端口37。   
> 接收32位的时间。   
> 关闭连结。   


### NETTIME程序
Windows Sockets API，通常也称为WinSock，与Berkeley Sockets API兼容，因此，可以想象UNIX Socket程序代码可以顺利地拿到Windows上使用。Windows下更进一步的支持由对Berkeley Socket扩充的功能提供，其函数的形式是以WSA（「WinSock API」）为前缀。相关的概述和参考位于/Platform SDK/Networking and Distributed Services/Windows Sockets Version 2。    
程序NETTIME展示了使用WinSock API的方法。   
**参考NETTIME程序**    
外观上，该程序建立了一个非系统模态对话框。程序重新定义了窗口的尺寸，以便非系统模态对话框可以覆盖程序的整个窗口显示区域。对话框包括一个只读编辑区（程序用于写入文字信息）、一个「Select Server」按钮、一个「Set Correct Time」按钮和一个「Close」按钮。「Close」按钮用于终止程序。   
MainDlg中的szIPAddr变量用于储存服务器地址，内定是字符串「132.163.135.130」。「Select Server」按钮启动依据NETTIME.RC中的SERVERS模板建立的对话框。szIPAddr变量作为最后一个参数传递给DialogBoxParam。「Server」对话框列出了10个服务器（都是从NIST网站上逐字复制来的），这些服务器提供了我们感兴趣的服务。当使用者单击一个服务器时，ServerDlg将分析按钮文字，以获得相应的IP地址。新地址储存在szIPAddr变量中。    
当使用者按下「Set Correct Time」按钮时，按钮将产生一个WM_COMMAND消息，其中wParam的低字组等于IDOK。MainDlg中的IDOK处理是大部分Socket初始行为发生的地方。    
使用Windows Sockets API时，任何Windows程序必须调用的第一个函数是：    
```c
iError = WSAStartup (wVersion, &WSAData) ;   
```   
第一个参数设定为0x0200（表示2.0版本）。传回时，WSAData结构包含了Windows Sockets实作的相关信息，而且NETTIME将显示szDescription字符串，并简要提供了一些版本信息。     
然后，NETTIME如下调用socket函数：   
```c
sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP) ;   
```   
**第一个参数是一个地址种类，表示此处是某种Internet地址。第二个参数表示数据以数据流的形式传回，而不是以数据封包的形式传回（我们需要的数据只有4个字节长，而数据封包适用于较大的数据块）。最后一个参数是一个协议，我们指定使用的Internet协议是TCP。它是RFC-868所定义的两个协议之一。socket函数的传回值储存在SOCKET型态的变量中，以便后面的Socket函数的呼叫。**    
之后调用的WSAAsynchSelect函数是另一个Windows特有的Socket函数。此函数用于避免因Internet响应过慢而造成应用程序当住。在WinSock文件中，有些函数与「阻碍性（blocking）」有关。也就是说，它们不能保证立即把控件权传回给程序。WSAAsyncSelect函数强制阻碍性的函数转为非阻碍性的，即在函数执行完之前把控件传回给程序。函数的结果以消息的形式报告给应用程序。WSAAsyncSelect函数让应用程序指定消息和接收消息的窗口的数值。通常，函数的语法如下：   
```c
WSAAsyncSelect (sock, hwnd, message, iConditions) ;   
```   
为此任务，NETTIME使用程序定义的一个消息，该消息称为WM_SOCKET_NOTIFY。它也用WSAAsyncSelect的最后一个参数来指定消息发送的条件，特别在连结和接收资料时（FD_CONNECT | FD_READ）。   
NETTIME调用的下一个WinSock函数是connect。这个函数需要一个指向Socket地址结构的指针，对于不同的协议来说，此Socket地址结构是不同的。NETTIME使用为TCP/IP设计的结构版本：   
```c
struct sockaddr_in    
{    
	short sin_family;    
	u_short sin_port;    
	struct in_addr sin_addr;    
	char sin_zero[8];     
} ;       
```    
其中in_addr是用于指定Internet地址，它可以用4个字节，或者2个无正负号短整数，或者1个无正负号长整数来表示。    
NETTIME将sin_family字段设定为AF_INET，用于表示地址种类。将sin_port设定为埠号，这里是时间协议的埠号，RFC-868显示为37。但不要像我最初时那样，将此字段设为37。当大多数数字通过Internet时，结构的这个端口号字段必须是「big endian」的，即最高的字节排第一个。Intel微处理器是little endian。幸运的是，htons（「host-to-network short」）函数使字节翻转，因此NETTIME将sockaddr_in结构的sin_port字段设定为：    
```c
htons (IPPORT_TIMESERVER)     
```   
WINSOCK2.H中将常数定义为37。NETTIME用inet_addr函数将储存在szIPAddr字符串中的服务器地址转化为无正负号长整数，该整数用于设定结构的sin_addr字段。    
connect函数通常已经会阻碍着后面程序的执行，这是因为连结成功以前需要花些时间。然而，由于NETTIME呼叫了WSAAsyncSelect，所以connect不会等待连结，事实上，它会立即传回SOCKET_ERROR的值。这并不是出现了错误，这只是表示现在还没有联机成功而已。NETTIME也不会检查这个传回值，只是呼叫WSAGetLastError而已。如果WSAGetLastError传回WSAEWOULDBLOCK（即函数的执行通常要受阻，但这里并没有受阻），那就一切都还很正常。NETTIME将「Set Correct Time」按钮改成「Cancel」，并设定了一个1秒的定时器。WM_TIMER的处理方式只是在程序窗口中显示句点，以告诉使用者程序仍在执行，系统没有当掉。     
连结最终完成时，MainDlg由WM_SOCKET_NOTIFY消息(在WSAAsyncSelect函数中指定的程序自订消息)通知。lParam的低字组等于FD_CONNECT，高字组表示错误。这时的错误可能是程序不能连结到指定的服务器。NETTIME还列出了其它9个服务器，供您选择，让您可以试试其它的服务器。    
如果一切顺利，那么NETTIME将继续调用recv（「receive：接收」）函数来读取数据：   
```c
recv (sock, (char *) &ulTime, 4, MSG_PEEK) ;   
```   
4意味着，用4个字节来储存ulTime变量。最后一个参数表示只是读此数据，并不将其从输入队列中删除。像connect函数一样，recv返回一个错误代码，以表示函数受阻，但这时没有受阻。理论上来说（当然这不大可能），函数至少能传回数据的一部分，然后通过再次调用函数以获得其余的32个字节值。那就是呼叫recv函数时带有MSG_PEEK选项的原因。  
与connect函数类似，recv函数也产生WM_SOCKET_NOTIFY消息，这时带有FD_READ的事件代码。程序通过再次调用recv来对此响应，这时最后的参数是0，用于从队列中删除数据。   
再次接收的32位的ulTime值是从1990年1月1日开始的0:00 UTC秒数。但最高顺序的字节是第一个字节，因此该值必须通过ntohl（「network-to-host long」）函数来调整字节顺序，以便Intel微处理器能够处理。然后，程序调用ChangeSystemTime函数。   
ChangeSystemTime首先取得目前的本地时间（即使用者所在时区的当前系统时间）。将SYSTEMTIME结构设定为1900年1月1日午夜（0时）。并将这个SYSTEMTIME结构传递给SystemTimeToFileTime，将此结构转化为FILETIME结构。FILETIME实际上只是由两个32位的DWORD一起组成64位的整数，用来表示从1601年1月1日至今间隔为100纳秒的间隔数。    
ChangeSystemTime函数将FILETIME结构转化为LARGE_INTEGER。它是一个union，允许64位的值可以被当成两个32位的值使用，或者当成一个__int64数据型态的64位整数使用（__int64数据型态是Microsoft编译器对ANSI C标准的扩充）。因此，此值是1601年1月1日到1900年1月1日之间间隔为100纳秒的间隔数。这里，添加了1900年1月1日至今间隔为100纳秒的间隔数－ulTime的10,000,000倍。    
然后通过调用FileTimeToSystemTime将作为结果的FILETIME值转换回SYSTEMTIME结构。因为时间协议传回目前的UTC时间，所以NETTIME通过调用SetSystemTime来设定时间，SetSystemTime也依据UTC。然后程序调用GetLocalTime来获得更新时间。最初的本地时间和新的本地时间一起传递给FormatUpdatedTime，这个函数用GetTimeFormat函数和GetDateFormat函数将时间转化为ASCII字符串。    
如果程序在Windows NT下执行，并且使用者没有取得设定时间的权限，那么SetSystemTime函数可能失败。如果SetSystemTime失败，则NETTIME将发出一个新时间未设定成功的消息来指出问题所在。