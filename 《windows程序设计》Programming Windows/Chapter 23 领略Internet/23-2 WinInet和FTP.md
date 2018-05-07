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
**参考UPDDEMO**     
UPDDEMO使用的文件名称是UDyyyymm.TXT，其中yyyy是4位阿拉伯数字的年数（当然适用于2000），mm是2位阿拉伯数字的月数。这里假定程序可以享有每个月都有更新文件的好处。这些文件可能是整个月刊，而由于阅读效率上的考虑，让程序将其下载到本地储存媒体上。    
因此，WinMain在呼叫ShowWindow和UpdateWindow来显示UPDDEMO主窗口以后，向WndProc发送程序定义的WM_USER_CHECKFILES消息。WndProc通过获得目前的年、月并检查该年月UDyyyymm.TXT文件所在的内定目录来处理此消息。这种文件的存在意义在于UPDDEMO会被完全更新（当然，事实并非如此。一些过时的文件将漏掉。如果要做得更完整，程序得进行更广泛的检测）。在这种情况下，UPDDEMO向自己发送一个WM_USER_GETFILES消息，它通过呼叫GetFileList函数来处理。这是UPDDEMO.C中稍长的一个函数，但它并不是特别有用，它所做的全部工作就是将所有的UDyyyymm.TXT文件读到动态配置的FILELIST型态结构中，该结构是在程序顶部定义的，然后让程序在其显示区域显示这些文件的内容。    
如果UPDDEMO没有最新的文件，那么它必须透过Internet进行更新。程序首先询问使用者这样做是否「OK」。如果是，程序将显示一个简单的对话框，其中只有一个「Cancel」按钮和一个ID为IDC_STATUS的静态文字区。下载时，此静态文字区向使用者提供状态报告，并且允许使用者取消过于缓慢的更新作业。此对话程序的名称是DlgProc。    
DlgProc很简单，它建立了一个包括自身窗口句柄的PARAMS型态的结构以及一个名称为bContinue的BOOL变量，然后呼叫_ beginthread来执行第二个线程。    
FtpThread函数透过使用下面的呼叫来完成实际的传输：InternetOpen、InternetConnect、FtpSetCurrentDirectory、FtpFindFirstFile、InternetFindNextFile、FtpGetFile和InternetCloseHandle（三次）。如同大多数程序代码，该线程函数如果略过错误检查、让使用者了解下一步的操作情况以及允许使用者随意取消整个显示的那些步骤，那么它将变得简洁许多。FtpThread函数透过用hwndStatus句柄呼叫SetWindowText来让使用者知道进展情况，这里指的是对话框中间的静态文字区。    
线程可以依照下面的三种方式之一来终止：    
第一种，FtpThread可能遇到从WinInet函数传回的错误。如果是这样，它将清除并编排错误字符串的格式，然后将此字符串（连同对话框文字区句柄和「Cancel」按钮的句柄一起）传递给ButtonSwitch。ButtonSwitch是一个小函数，它显示了文字字符串，并将「Cancel」按钮转换成「OK」按钮－不只是按钮上的文字字符串的转换，还包括控件ID的转换。这样就允许使用者按下「OK」按钮来结束对话框。     
第二种方式，FtpThread能在没有任何错误的情况下完成任务，其处理方法和遇到错误时的方法一样，只不过对话框中显示的字符串为「Internet Download Complete」。    
第三种方式，使用者可以在程序中选择取消下载。这时，DlgProc将PARAMS结构的bContinue字段设定为FALSE。FtpThread频繁地检查该值，如果bContinue等于FALSE，那么函数将做好应该进行的收拾工作，并以NULL文字参数呼叫ButtonSwitch，此参数表示显示了字符串「Internet Session Cancelled」。同样，使用者必须按下「OK」按钮来关闭对话框。    
虽然UPDDEMO取得的每个文件只能显示一行，但我（本书的作者）可以用这个程序来告诉您（本书的读者）本书的更新内容以及其它信息，您也可以在网站上发现更详细的信息。因此，UPDDEMO成为我向您传送信息的方法，并且可以让本书的内容延续到最后一页之后。（这算是广告嘛）    
**这个程序大部分内容已经了解，唯一的问题是InternetOpen函数返回了NULL代表了失败，书上未写，百度上也没有结果。推测是该FTP服务器已经关闭了。等以后自己会创建FTP服务器再进行尝试确认。**    
整本结束。（撒花