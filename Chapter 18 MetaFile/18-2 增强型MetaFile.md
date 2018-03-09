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
EMF3说明了：metafile只会记录“会使用到设备内容句柄的GDI函数”并不会记录“不需要设备内容句柄的”函数。  
比如：  
```c
SelectObject(hdcEMF, CreateSolidBrush(RGB(0,0,255)));  
```  
CreateSolidBrush函数调用并不会被记录到metafile中但是selectobject函数会被记录进去。   
总结一下，每当非内定的GDI对象首次被选入MetaFile设备内容时，GDI都会为该对象建立函数的记录编码。每个对象有一个依序从1开始的唯一数值，此数值由记录的第三个字段表示。跟在此记录后的是引用该数值的EMR_SELECTOBJECT记录。以后，将对象选入MetaFile设备内容时（在中间时期没有被删除），就只需要EMR_SELECTOBJECT记录了。
### MetaFile和位图
现在，让我们做点稍微复杂的事，在MetaFile设备内容中绘制一幅位图，如程序18-5 EMF4所示。   
参考EMF4   
为了方便，EMF4加载由常数OEM_CLOSE指出的系统位图。在设备内容中显示位图的惯用方法是通过使用CreateCompatibleDC建立与目的设备内容（此例为MetaFile设备内容）兼容的内存设备内容。然后，通过使用SelectObject将位图选入该内存设备内容并且从该内存设备内容呼叫BitBlt或StretchBlt把位图画到目的设备内容。结束后，删除内存设备内容和位图。
### 列举MetaFile内容
**当希望存取MetaFile内的个别记录时**，可以使用称作MetaFile列举的程序。如程序18-6 EMF5所示。此程序使用MetaFile来显示与EMF3相同的图像，但它是通过MetaFile列举来进行的。  
参考EMF5   
EMF5使用EMF3程序建立的EMF3.EMF文件，所以确定在执行此程序前先执行EMF3程序。  
在处理WM_PAINT时，两个程序的主要区别是EMF3呼叫PlayEnhMetaFile，而EMF5呼叫EnumEnhMetaFile。PlayEnhMetaFile函数有下面的语法：  
```c
PlayEnhMetaFile (hdc, hemf, &rect) ;   
``` 
第一个参数是要显示的MetaFile的设备内容句柄。第二个参数是增强型MetaFile句柄。第三个参数是指向描述设备内容平面上矩形的RECT结构的指针。MetaFile图像大小被缩放过，以便刚好能够显示在不超过该矩形的区域内。    
EnumEnhMetaFile有5个参数，其中3个与PlayEnhMetaFile一样（虽然RECT结构的指针已经移到参数表的末尾）。   
EnumEnhMetaFile的第三个参数是列举函数的名称，它用于呼叫EnhMetaFileProc。第四个参数是希望传递给列举函数的任意数据的指针，这里将该参数简单地设定为NULL。   
列举函数。当调用EnumEnhMetaFile时，**对于MetaFile中的每一个记录，GDI都将调用EnhMetaFileProc一次（意味着将多次重复的调用）**，包括表头纪录和文件结束记录。通常列举函数传回TRUE，但它可能传回FALSE以略过剩下的列举程序。   
**EMF5使用EnumEnhMetaFile和PlayEnhMetaFileRecord得到的结果与EMF3呼叫PlayEnhMetaFile得到的结果一样。区别在于EMF5现在直接介入了MetaFile的显示程序，并能够存取各个MetaFile记录。这是很有用的。**   
列举函数(EnhMetaFileProc)的第一个参数是设备内容句柄。GDI从EnumEnhMetaFile的第一个参数中简单地取得此句柄。列举函数把该句柄传递给PlayEnhMetaFileRecord来标识图像显示的目的设备内容。   
我们先跳到列举函数的第三个参数，它是指向ENHMETARECORD型态结构的指针，前面已经提到过。这个结构描述实际的MetaFile记录，就像它亲自在MetaFile中编码一样。    
可以写一些程序代码来检查这些记录。如果不想把某些记录传送到PlayEnhMetaFileRecord函数。例如，在EMF5.C中，把下行插入到PlayEnhMetaFileRecord呼叫的前面：   
```c
if (pEmfRecord->iType != EMR_LINETO)   
```  
重新编译程序，执行它，将只看到矩形，而没有两条线。或使用下面的叙述：   
```    
if (pEmfRecord->iType != EMR_SELECTOBJECT)   
```   
这个小改变会让GDI用内定对象显示图像，而不是用MetaFile所建立的画笔和画刷。   
可以像上面一样，阻止将metafile的内容传递给PlayEnhMetaFileRecord函数，但是程序中不应该直接的修改MetaFile记录。   
以下看一下程序EMF6。   
参考EMF6   
与EMF5一样，EMF6使用EMF3程序建立的EMF3.EMFMetaFile，因此要在Visual C++中执行这个程序之前先执行过EMF3程序。  
EMF5中有筛选的将metafile中的记录显示了出来，EMF6中更改了metafile中的记录。  
但是在emf6中并不是更改原始记录，而是申请内存做了一个副本，先复制到副本，然后对副本进行操作。   
在列举函数EnhMetaFileProc中的倒数第二个参数是nHandles，通常它是比MetaFile中简历的GDI对象还要大的值。它代表着GDI对象以及多余的一个是所列举的MetaFile句柄。当然它仅仅是一个数量。   
而列举函数EnhMetaFileProc中的第二个参数是HANDLETABLE结构的指针，定义如下：   
```c   
typedef struct tagHANDLETABLE  
{   
	HGDIOBJ objectHandle [1] ;   
}    
HANDLETABLE ;   
```   
其中objectHandle数组中的元素等于nHandles的数量。   
在EMF6中这个数量是3，代表着画笔、画刷和MetaFile句柄。而其中，每次呼叫列举函数时，数组的**第一个元素**都将包含所列举的MetaFile句柄。   
在列举函数中，可以使用以下表达式取得这些GDI对象句柄：   
```c
pHandleTable->objectHandle[i]   
```   
**在第一次呼叫列举函数时**，表的第二、第三个元素将是0。它们是画笔和画刷句柄的保留位置。    
以下是列举函数运作的方式：MetaFile中的第一个对象建立函数具有EMR_CREATEBRUSHINDIRECT的记录型态，此记录指出了对象编号1。当把该记录传递给PlayEnhMetaFileRecord时，GDI建立画刷并取得它的句柄。此句柄储存在objectHandle数组的元素1（第二个元素）中。当把第一个EMR_SELECTOBJECT记录传递给PlayEnhMetaFileRecord时，GDI发现此对象编号为1，并能够从表中找到该对象实际的句柄，而把它用来呼叫SelectObject。当MetaFile最后删除画刷时，GDI将objectHandle数组的元素1设定回0。   
通过存取objectHandle数组，可以使用例如GetObjectType和GetObject等呼叫取得在MetaFile中使用的对象信息。   
### 嵌入图像
列举MetaFile的最重要应用也许是在现有的MetaFile中嵌入其它图像（甚至是整个MetaFile）。事实上，现有的MetaFile保持不变；真正进行的是建立包含现有MetaFile和新嵌入图像的新MetaFile。基本的技巧是把MetaFile设备内容句柄传递给EnumEnhMetaFile，作为它的第一个参数。   
在MetaFile命令序列的开头或结尾嵌入新图像是极简单的－就在EMR_HEADER记录之后或在EMF_EOF记录之前。然而，如果熟悉现有的MetaFile结构，就可以把新的绘图命令嵌入所需的任何地方。如程序EMF7所示。    
参考EMF7   
EMF7使用EMF3程序建立的EMF3.EMF，所以在执行EMF7之前要执行EMF3程序建立MetaFile。**将emf3的内容读取后，以列举的方式进行一定的修改并且存入emf7中，最后显示出来。（这是本程序最主要的新内容，以下为详解）**   
首先，程序通过呼叫GetEnhMetaFile取得EMF3.EMF文件的MetaFile句柄，还呼叫GetEnhMetaFileHeader得到增强型MetaFile表头记录，目的是在后面的EnumEnhMetaFile调用中使用rclBounds字段。   
> EnumEnhMetaFile的第三个参数是指向描述设备内容平面上矩形的RECT结构的指针。MetaFile图像大小被缩放过，以便刚好能够显示在不超过该矩形的区域内。   


接下来，程序建立新的MetaFile文件EMF7.EMF。CreateEnhMetaFile函数传回设备内容句柄。然后，使用EMF7.EMF的MetaFile设备内容句柄和EMF3.EMF的MetaFile句柄调用EnumEnhMetaFile。   
现在来看一看EnhMetaFileProc。如果被列举的记录不是表头纪录或文件结束记录，函数就呼叫PlayEnhMetaFileRecord把记录转换（画进去、复制进去）为新的MetaFile设备内容（并不是一定要排除表头纪录或文件结束记录，但它们会使MetaFile变大）。   
而如果转换的记录是画矩形，则函数建立画笔用绿色的轮廓线和透明的内部来绘制椭圆。  
回到WM_CREATE的最后，程序调用CloseEnhMetaFile取得新MetaFile的句柄。然后，它删除两个MetaFile句柄，而EMF3.EMF和EMF7.EMF文件留在了磁盘上。    
### 增强型MetaFile阅览器和打印机
使用剪贴簿转换增强型MetaFile很简单，剪贴簿型态是CF_ENHMetaFile。GetClipboardData函数传回增强型MetaFile句柄，SetClipboardData也使用该MetaFile句柄。复制MetaFile时可以使用CopyEnhMetaFile函数。如果把增强型MetaFile放在剪贴簿中，Windows会让需要旧格式的那些程序也可以使用它。如果在剪贴簿中放置旧格式的MetaFile，Windows将也会自动视需要把内容转换为增强型MetaFile的格式。   
程序EMFVIEW所示为在剪贴簿中传送MetaFile的程序代码，它也允许载入、储存和打印MetaFile。   
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