## WinInet和FTP
WinInet（「Windows Internet」）API是一个高阶函数集，帮助程序写作者使用三个常见的Internet协议，这三个协议是：   
1 用于World Wide Web全球信息网的超文字传输协议（HTTP：Hypertext Transfer Protocol）    
2 文件传输协议（FTP：File Transfer Protocol）    
3 称为Gopher的文件传输协议。    
WinInet函数的语法与常用的Windows文件函数的语法类似，这使得使用这些协议就像使用本地磁盘驱动器上的文件一样容易。WinInet API的文件位于/Platform SDK/Internet, Intranet, Extranet Services/Internet Tools and Technologies/WinInet API。    
下面的范例程序将展示如何使用WinInet API的FTP部分。    
许多有网站的公司也都有「匿名FTP」服务器，这样使用者可以在不输入使用者名称和密码的情况下下载文件。例如，如果在Internet Explorer的地址栏输入ftp://ftp.microsoft.com，那么就可以浏览FTP服务器上的目录并下载文件。如果进入ftp://ftp.cpetzold.com/cpetzold.com/ProgWin/UpdDemo，那么您将在我的匿名FTP服务器上发现与待会要提到的范例程序一块使用的文件列表。     
虽然现今FTP服务对大多数的Web使用者来说并不是那么方便使用，但它仍然相当有用。例如，应用程序能利用FTP从匿名FTP服务器上取得数据，这些取得数据的运作程序几乎完全在台面下处理，而不需要使用者操心。这就是我们将讨论的UPDDEMO（「update demonstration：更新范例」）程序的构想。
### FTP API概况
**使用WinInet的程序必须在所有呼叫WinInet函数的源文件中包括表头文件WININET.H。程序还必须连结WININET.LIB。**    
在下面的论述中，我不会详细讨论函数的语法，因为某些函数有很多选项，这让它变得相当复杂。要掌握WinInet，您可以将UPDDEMO原始码当成食谱来看待。**这时最重要的是了解有关的各个步骤以及FTP函数的范围。**    
要使用Windows Internet API，首先要呼叫InternetOpen。然后，使用WinInet支持的任何一种协议。InternetOpen将返回一个Internet作业句柄，并储存到HINTERNET型态的变量中。用完WinInet API以后，应该通过调用InternetCloseHandle来关闭句柄。    
要使用FTP，接下来就要调用InternetConnect。此函数需要使用作业句柄（由InternetOpen返回的FTP作业句柄）。可将此句柄视为所有名称开头为Ftp的所有函数的第一个参数。    
InternetConnect函数的参数指出要使用的FTP，还提供了服务器名称，例如，ftp.cpetzold.com。此函数还需要使用者名称和密码。如果存取匿名FTP服务器，这些参数可以设定为NULL。如果应用程序调用InternetConnect时PC并没有连结到Internet，Windows 98将显示「拨号联机」对话框。当使用FTP的应用程序结束时，调用InternetCloseHandle来关闭句柄。      
这时候可以开始调用有Ftp前缀的函数了。这些函数与标准的Windows文件I/O函数很相似。为了避免与其它协议重复，一些以Internet为前缀的函数也可以处理FTP。    
下面四个函数用于处理目录：    
```c   
fSuccess = FtpCreateDirectory (hFtpSession, szDirectory) ;    
fSuccess = FtpRemoveDirectory (hFtpSession, szDirectory) ;     
fSuccess = FtpSetCurrentDirectory (hFtpSession, szDirectory) ;    
fSuccess = FtpGetCurrentDirectory (hFtpSession, szDirectory, &dwCharacterCount) ;     
```    
些函数很像我们所熟悉的Windows提供用于处理本地文件系统的CreateDirectory、RemoveDirectory、SetCurrentDirectory和GetCurrentDirectory函数。     
当然，存取匿名FTP的应用程序不能建立或删除目录。而且，程序也不能假定FTP目录具有和Windows文件系统相同的目录结构型态。特别是用相对路径名设定目录的程序，不能假定关于新的目录全名的一切。如果程序需要知道最后所在目录的整个名称，那么调用了SetCurrentDirectory之后必须再调用GetCurrentDirectory。GetCurrentDirectory的字符串参数至少包含MAX_PATH字符，并且最后一个参数应指向包含该值的变量。    
下面两个函数用来删除或者重新命名文件（但不是在匿名FTP服务器上）：  
```c
fSuccess = FtpDeleteFile (hFtpSession, szFileName) ;   
fSuccess = FtpRenameFile (hFtpSession, szOldName, szNewName) ;    
```    
经由先呼叫FtpFindFirstFile，可以查找文件（或与含有万用字符的文件名样式相符的多个文件）。此函数很像FindFirstFile函数，甚至都使用了相同的WIN32_FIND_DATA结构。该文件为列举出来的文件传回了一个句柄。您可以将此句柄传递给InternetFindNextFile函数以获得额外的文件名称信息。最后通过调用InternetCloseHandle来关闭句柄。    
要打开文件，可以调用FtpFileOpen。这个函数传回一个文件句柄，此句柄可以用于InternetReadFile、InternetReadFileEx、InternetWrite和InternetSetFilePointer呼叫。同样，最后可以通过调用最常用的InternetCloseHandle函数来关闭句柄。    
最后，下面两个高级函数特别有用：FtpGetFile调用将文件从FTP服务器复制到本地内存，它合并了FtpFileOpen、FileCreate、InternetReadFile、WriteFile、InternetCloseHandle和CloseHandle调用。FtpGetFile的另一个参数是一个旗标，如果本地已经存在同名文件，那么该旗标将导致函数呼叫失败。FtpPutFile与此函数类似，用于将文件从本地内存复制到FTP服务器。       
### 更新展示程序
程序UPDDEMO，展示了用WinInet FTP函数在第二个线程执行期间从匿名FTP服务器上下载文件的方法。    
参考UPDDEMO