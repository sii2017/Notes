# 第二十二章 声音与音乐
## windows和多媒体
从某种意义上来说，多媒体就是透过与设备无关的函数呼叫来获得对各种硬件的存取。  
### 多媒体硬件
或许最常用的多媒体硬件就是波形声音设备，也就是平常所说的**声卡**。波形声音设备将麦克风的输入或其它声音输入转换为数字取样，并将其储存到内存或者储存到以.WAV为扩展名的磁盘文件中。波形声音设备还将波形转换回模拟声音，以便通过PC扩音器来播放。    
**声卡通常还包含MIDI设备**。MIDI是符合工业标准的乐器数字化接口（Musical Instrument Digital Interface）。这类硬件播放音符以响应短的二进制命令消息。MIDI硬件通常还可以通过电缆连结到如音乐键盘等的MIDI输入设备上。通常，外部的MIDI合成器也能够添加到声卡上。    
现在，大多数PC上的CD-ROM驱动器都具备播放普通音乐CD的能力。这就是平常所说的「CD声音」。     
来自波形声音设备、MIDI设备以及CD声音设备的输出，一般在使用者的控制下用「音量控制」程序混合在一起。    
另外几种普遍的多媒体「设备」**不需要额外的硬件**。Windows视讯设备（也称作AVI视讯设备）播放扩展名为.AVI（audio-video interleave：声音视频插格）的电影或动画文件。「ActiveMovie控件」可以播放其它型态的电影，包括QuickTime和MPEG。PC上的显示卡需要特定的硬件来协助播放这些电影。   
### API概述
在Windows中，API支持的多媒体功能主要分成两个集合。它们通常称为「低阶」和「高阶」界面。    
低阶接口是一系列函数，这些函数以简短的说明性前缀开头，而且在/Platform SDK/Graphics and Multimedia Services/Multimedia Reference/Multimedia Functions（与高阶函数一起）中列出。       
**低阶的波形声音输入输出函数的前缀是waveIn和waveOut。**另外midiOut函数用来控制MIDI输出设备。这些API还包括midiIn和midiStream函数。    
您还会注意到多媒体函数列表中七个带有前缀mci的函数，它们允许存取媒体控制接口（MCI：Media Control Interface）。**这是一个高阶的开放接口，用于控制多媒体PC中所有的多媒体硬件。**MCI包括所有多媒体硬件都共有的许多命令，因为多媒体的许多方面都以磁带录音机这类设备播放/记录方式为模型。您为输入或输出而「打开」一台设备，进而可以「录音」（对于输入）或者「播放」（对于输出），并且结束后可以「关闭」设备。     
MCI本身分为两种形式。一种形式下，可以向MCI发送消息，这类似于Windows消息。这些消息包括位编码标记和C数据结构。另一种形式下，可以向MCI发送文字字符串。这个程序主要用于描述命令语言，此语言具有灵活的字符串处理函数，但支持呼叫Windows API的函数不多。    
存取多媒体硬件的另一种方法包括DirectX API。（它超出了本书的范围）   
另外两个高阶多媒体函数也值得一提：MessageBeep和PlaySound，它们在第三章有示范。MessageBeep播放「控制台」的「声音」中指定的声音。PlaySound可播放磁盘上、内存中或者作为资源加载的.WAV文件。    
### 用TESTMCI研究MCI
在Windows多媒体的早期，软件开发套件含有一个名为MCITEST的C程序，它允许程序写作者交谈式输入MCI命令并学习这些命令的工作方式。这个程序是C语言版，现在已经消失了。因此，我们将重新建立它，即TESTMCI程序。目前程序代码与旧的程序代码没有什么区别，但现在的使用者接口还是依据以前的MCITEST程序，并且没有使用现在新的接口。     
**参考TESTMCI**     
需要在项目的properties中linker的addition depends中添加WINMM.lib才能正确调用mci相关的函数。相关lib和dll应该都在c盘的固定路径里。   
此程序用到了两个最重要的多媒体函数：mciSendString和mciGetErrorText。在TESTMCI的主编辑窗口输入一些内容然后按下Enter键（或「OK」按钮）后，程序将输入的字符串作为第一个参数传递给mciSendString命令：    
```c
error = mciSendString (szCommand, szReturn,
sizeof (szReturn) / sizeof (TCHAR), hwnd) ;   
```     
如果在编辑窗口选择了不止一行，则程序将按顺序将它们发送给mciSendString函数。第二个参数是字符串地址，此字符串取得从函数传回的信息。程序将此信息显示在窗口的「Return String」区域。从mciSendString传回的错误代码传递给mciGetErrorString函数，以获得文字错误说明；此说明显示在TESTMCI窗口的「Error String」区域。
### MCITEXT和CD声音
通过控制CD-ROM驱动器和播放声音CD，这些命令字符串一般都非常简单。您可以在/Platform SDK/Graphics and Multimedia Services/Multimedia Reference/Multimedia Command Strings中获得MCI命令字符串的参考。   
> 这里要求放入cd，并且使用上一个程序来播放。实际上我们现在已经不用光驱了。（2018年）**因此以下内容并没有实际意义，仅作留存使用。可以直接跳到下一章**   

放入光盘后，使用上面的程序TESTMCI并且键入命令：open cdaudio。   
其中open是MCI命令，cdaudio是MCI认定的CD-ROM驱动器的设备名称（假定您的系统中只有一个CD-ROM驱动器。要获得多个CD-ROM驱动器名称需使用sysinfo命令）。    
TESTMCI中的「Return String」区域显示mciSendString函数中系统传回给程序的字符串。如果执行了open命令，则此值是1。TESTMCI在「Error String」区域中显示mciGetErrorString依据mciSendString传回值所传回的信息。如果mciSendString没有传回错误代码，则「Error String」区域显示文字"The specified command was carried out"。    
假定执行了open命令，现在就可以输入：   
play cdaudio   
CD将开始播放唱片上的第一首乐曲「Thunder Road」。输入下面的命令可以暂停播放：    
pause cdaudio    
或者   
stop cdaudio    
对于CD声音设备来说，这些叙述的功能相同。您可用下面的叙述重新播放：   
play cdaudio   
迄今为止，我们使用的全部字符串都由命令和设备名称组成。其中有些命令带有选项。例如，键入：     
status cdaudio position    
根据收听时间的长短，「Return String」区域将显示类似下面的一些字符：    
01:15:25    
这是些什么？很显然不是小时、分钟和秒，因为CD没有那么长。要找出时间格式，请键入：   
status cdaudio time format    
现在「Return String」区域显示下面的字符串：   
msf     
这代表「分-秒-格」。CD声音中，每秒有75格。时间格式的讯格部分可在0到74之间的范围内变化。    
状态命令有一连串的选项。使用下面的命令，您可以确定msf格式的CD全部长度：    
status cdaudio length    
对于「Born to Run」，「Return String」区域将显示：   
39:28:19   
这指的是39分28秒19格。    
现在试一下    
status cdaudio number of tracks   
「Return String」区域将显示：    
8    
我们从CD封面上知道「Born to Run」CD上第五首乐曲是主题曲。MCI命令中的乐曲从1开始编号。要想知道乐曲「Born to Run」的长度，可以键入下面的命令：   
status cdaudio length track 5   
「Return String」区域将显示：   
04:30:22   
我们还可确定此乐曲从盘上的哪个位置开始：    
status cdaudio position track 5   
「Return String」区域将显示：   
17:36:35    
根据这条信息，我们可以直接跳到乐曲标题：    
play cdaudio from 17:36:35 to 22:06:57    
此命令只播放一首乐曲，然后停止。最后的值是由4:30:22（乐曲长度）加17:36:35得到的。或者，也可以用下面的命令确定：   
我们从CD封面上知道「Born to Run」CD上第五首乐曲是主题曲。MCI命令中的乐曲从1开始编号。要想知道乐曲「Born to Run」的长度，可以键入下面的命令：    
status cdaudio length track 5  
「Return String」区域将显示：   
04:30:22    
我们还可确定此乐曲从盘上的哪个位置开始：   
status cdaudio position track 5   
「Return String」区域将显示：   
17:36:35    
根据这条信息，我们可以直接跳到乐曲标题：   
play cdaudio from 17:36:35 to 22:06:57    
此命令只播放一首乐曲，然后停止。最后的值是由4:30:22（乐曲长度）加17:36:35得到的。或者，也可以用下面的命令确定：    
直到整个唱片播放完以后，mciSendString函数才将控制权传回给程序。如果必须使用wait选项（在只要执行MCI描述文件而不管其它事情的时候，这么做很方便，与我将展示的一样），首先使用break命令。此命令可设定一个虚拟键码，此码将中断mciSendString命令并将控制权传回给程序。例如，要设定Escape键来实作此目的，可用：   
break cdaudio on 27   
这里，27是十进制的VK_ESCAPE值。    
比wait选项更好的是notify选项：   
play cdaudio from 5:0:0 to 5:0:10 notify   
这种情况下，mciSendString函数立即传回，但如果该操作在MCI命令的尾部定义，则mciSendString函数的最后一个参数所指定句柄的窗口会收到MM_MCINOTIFY消息。TESTMCI程序在MM_MCINOTIFY框中显示此消息的结果。为避免与其它可能键入的命令混淆，TESTMCI程序在5秒后停止显示MM_MCINOTIFY消息的结果。    
您可以同时使用wait和notify关键词，但没有理由这么做。不使用这两个关键词，内定的操作就既不是wait，也不是您通常所希望的notify。   
用这些命令结束播放时，可键入下面的命令来停止CD：   
stop cdaudio   
如果在关闭之前没有停止CD-ROM设备，那么甚至在关闭设备之后还会继续播放CD。    
另外，您还可以试试您的硬件允许或者不允许的一些命令：   
eject cdaudio    
最后按下面的方法关闭设备：   
close cdaudio     
虽然TESTMCI自己不能储存或加载文本文件，但可以在编辑控件和剪贴簿之间复制文字：先从TESTMCI选择一些内容，将其复制到剪贴簿（用Ctrl-C），再将这些文字从剪贴簿复制到「记事本」，然后储存。相反的操作，可以将一系列的MCI命令加载到TESTMCI。如果选择了一系列命令然后按下「OK」按钮（或者Enter键），则TESTMCI将每次执行一条命令。这就允许您编写MCI的「描述文件」，即MCI命令的简单列表。    
例如，假设您想听歌曲「Jungleland」（唱片中的最后一首）、「Thunder Road」和「Born to Run」，并要按此顺序听，可以编写如下的描述命令：    
open cdaudio    
set cdaudio time format tmsf  
break cdaudio on 27   
play cdaudio from 8 wait   
play cdaudio from 1 to 2 wait   
play cdaudio from 5 to 6 wait   
stop cdaudio    
eject cdaudio   
close cdaudio   
不用wait关键词，就不能正常工作，因为mciSendString命令会立即传回，然后执行下一条命令。     
此时，如何编写仿真CD播放程序的简单应用程序，就应该相当清楚了。程序可以确定乐曲数量、每个乐曲的长度并能显示允许使用者从任意位置开始播放（不过，请记住：mciSendString总是传回文字字符串信息，因此您需要编写解析处理程序来将这些字符串转换成数字）。可以肯定，这样的程序还要使用Windows定时器，以产生大约1秒的时间间隔。在WM_TIMER消息处理期间，程序将呼叫：    
status cdaudio mode    
来查看CD是暂停还是在播放。  
status cdaudio position    
命令允许程序更新显示以给使用者显示目前的位置。但可能还存在更令人感兴趣的事：如果程序知道音乐音调部分的节拍位置，那么就可以使屏幕上的图形与CD同步。这对于音乐指令或者建立自己的图形音乐视讯程序极为有用。     
