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
