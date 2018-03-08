## 增强型MetaFile
「增强型MetaFile」格式是在32位Windows版本中发表的。它包含一组新的函数呼叫、一对新的数据结构、新的剪贴簿格式和新的文件扩展名.EMF。  
这种新的MetaFile格式最重要的改进是**加入可通过函数呼叫取得的更丰富的表头信息**，这种表头信息可用来帮助应用程序显示MetaFile图像。  
有些增强型MetaFile函数使您能够在增强型MetaFile(EMF)格式和老式MetaFile格式（也称作Windows MetaFile(WMF)格式）之间来回转换。当然，这种转换很可能遇到麻烦，因为老式MetaFile格式并不支持某些，例如GDI绘图路径等，新的32位图形功能。
### 基本程序
程序EMF1建立并显示增强型MetaFile。   
参考EMF1   
首先，该程序在建立增强型MetaFile时，画矩形和直线的函数所使用的坐标并不是实际意义上的坐标。您可以将它们同时加倍或都减去某个常数，而其结果不会改变。这些坐标只是在定义图像时说明彼此间的对应关系。   
其次，为了适于在传递给PlayEnhMetaFile函数的矩形中显示，图像大小会被缩放。因此，如图18-2所示，图像可能会变形。尽管MetaFile坐标指出该图像是正方形的，但一般情况下我们却得不到这样的图像。而在某些时候，这又正是我们想要得到的图像。例如，将图像嵌入一段文书处理格式的文字中时，可能会要求使用者为图像指定矩形，并且确保整个图像恰好位于矩形中而不浪费空间。这样，使用者可通过适当调整矩形的大小来得到正确的纵横比。   
### 解开内幕
如果看一看MetaFile的内容会对MetaFile工作的方式有一个更好的理解。EMF2程序建立了一个MetaFile。   
参考EMF2   
在EMF1程序中，CreateEnhMetaFile函数的所有参数均被设定为NULL。但是在EMF2中：  
```c
hdcEMF= CreateEnhMetaFile(NULL, TEXT("emf2.emf"), NULL, 
			TEXT("EMF2\OEMF Demo #2\0"));   
```   
第一个参数仍旧设定为NULL，该参数还可以是设备内容句柄。   
第二个参数是文件名称。如果该参数为NULL（在EMF1中为NULL，但在EMF2中不为NULL），则该函数建立内存MetaFile。EMF2建立名为EMF2.EMF的MetaFile文件。   
函数的第三个参数是RECT结构的地址，它指出了以0.01mm为单位的MetaFile的总大小。这是MetaFile表头数据中极其重要的信息（这是早期的Windows MetaFile格式的缺陷之一）。**如果该参数为NULL，GDI会计算出尺寸。当应用程序对性能要求比较严格时，就需要使用该参数以避免让GDI处理太多东西**。   
最后的参数是描述该MetaFile的字符串。该字符串分为两部分：第一部分是以NULL字符结尾的应用程序名称（不一定是程序的文件名称），第二部分是描述视觉图像内容的说明，以两个NULL字符结尾。例如用C中的符号「\0」作为NULL字符。由于在C中通常会在使用的字符串末尾放入一个NULL字符，所以如EMF2所示，在末尾仅需一个「\0」。   
由于不在内存中没有句柄可以使用，则通过函数GetEnhMetaFile存取这个文件。   
增强型MetaFile由可变长度的记录组成，这些记录的一般格式由ENHMETARECORD结构说明，它在WINGDI.H表头文件中定义如下：   
```c
typedef struct tagENHMETARECORD   
{   
	DWORD iType ; // record type   
	DWORD nSize ; // record size   
	DWORD dParm [1] ; // parameters   
}   
ENHMETARECORD ;   
```
那个只有一个元素的数组指出了数组元素的变量。参数的数量取决于记录型态。iType字段可以是定义在WINGDI.H文件中以前缀EMR_开始的近百个常数之一。nSize字段是总记录的大小，包括iType和nSize字段以及一个或多个dParm字段。
增强型MetaFile总是以表头纪录开始。它对应于ENHMETAHEADER型态的结构，定义如下：   
```c
typedef struct tagENHMETAHEADER   
{   
	DWORD iType ; // EMR_HEADER = 1   
	DWORD nSize ; // structure size  
	RECTL rclBounds ; // bounding rectangle in pixels   
	RECTL rclFrame ; // size of image in 0.01 millimeters   
	DWORD dSignature ; // ENHMETA_SIGNATURE = " EMF"   
	DWORD nVersion ; // 0x00010000    
	DWORD nBytes ; // file size in bytes   
	DWORD nRecords ; // total number of records   
	WORD nHandles ; // number of handles in handle table   
	WORD sReserved ;    
	DWORD nDescription ; // character length of description string   
	DWORD offDescription ; // offset of description string in file         
	DWORD nPalEntries ; // number of entries in palette  
	SIZEL szlDevice ; // device resolution in pixels   
	SIZEL szlMillimeters ; // device resolution in  millimeters    
	DWORD cbPixelFormat ; // size of pixel format  
	DWORD offPixelFormat ; // offset of pixel format  
	DWORD bOpenGL ; // FALSE if no OpenGL records   
}   
ENHMETAHEADER ;  
```  
这种表头纪录的存在可能是增强型MetaFile格式对早期Windows MetaFile所做的最为重要的改进。不需要对MetaFile文件使用文件I/O函数来取得这些表头信息。如果具有MetaFile句柄，就可以使用GetEnhMetaFileHeader函数：   
```c
GetEnhMetaFileHeader (hemf, cbSize, &emh) ;   
```  
第一个参数是MetaFile句柄。最后一个参数是指向ENHMETAHEADER结构的指针。第二个参数是该结构的大小。可以使用类似的GetEnh-MetaFileDescription函数取得描述字符串。   
### MetaFile与GDI物件
我们已经知道了GDI绘图命令储存在MetaFile中方式，现在看一下GDI对象的储存方式。程序EMF3除了建立用于绘制矩形和直线的非内定画笔和画刷以外，与前面介绍的EMF2程序很相似。该程序也对Rectangle坐标的问题提出了一点修改。EMF3程序使用GetVersion来确定执行环境是Windows 98还是Windows NT，并适当地调整参数。   
参考EMF3
### MetaFile和位图
参考EMF4
### 列举MetaFile内容
参考EMF5
参考EMF6  
### 嵌入图像
参考EMF7
### 增强型MetaFile阅览器和打印机
参考EMFVIEW
### 显示精确的MetaFile图像
参考EMF8
参考EMF9
### 缩放比例和纵横比
参考EMF10
### MetaFile中的映像方式
参考EMF11
### 映射与显示
参考EMF12
参考EMF13