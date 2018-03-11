# 第十八章 MetaFile（图元文件）
MetaFile和向量图形的关系，就像位图和位映像图形的关系一样。位图通常来自实际的图像，而MetaFile则大多是通过计算机程序人为建立的。MetaFile由一系列与图形函数呼叫相同的二进制记录组成，这些记录一般用于绘制直线、曲线、填入的区域和文字等。  
在画图（paint）程序中，我们只能将画出来的画面显示在显示区域内。但是在绘图（draw）程序中，我们可以抓住某个独立的图形对象（比如一条直线）并且将他移动到其它的位置。这是因为组成图形的每隔成员都是以单独的记录进行存储的。这在画图程序中是不可能做到的。   
**由于MetaFile以图形绘制命令描述图像**，因此可以对图像进行缩放而不会失真。位图则不然，如果以二倍大小来显示位图，您却无法得到二倍的分辨率，而只是在水平和垂直方向上重复位图的位。  
MetaFile可以转换为位图，但是会丢失一些信息：组成MetaFile的图形对象将不再是独立的，而是被合并进大的图像。将位图转换为MetaFile要艰难得多，一般仅限于非常简单的图像，而且它需要大量处理来分析边界和轮廓。而MetaFile可以包含绘制位图的命令。  
**由于MetaFile将图片描述为图像函数呼叫的集合，因而它们既比位图占用更少的空间，又比位图更与设备无关。**   
Microsoft Windows支持两种MetaFile格式和支持这些格式的两组函数。我首先讨论从Windows 1.0到目前的32位Windows版本都支持的MetaFile函数，然后讨论为32位Windows系统开发的「增强型MetaFile」。增强型MetaFile在原有MetaFile的基础上有了一些改进，应该尽可能地加以利用。   
## 旧的MetaFile格式 
MetaFile既能够暂时储存在内存中，也能够以文件的形式储存在磁盘上。对应用程序来说，两者区别不大。  
### 内存MetaFile的简单利用
如果呼叫CreateMetaFile函数来建立MetaFile设备内容，Windows就会以早期的格式建立一个MetaFile，然后您可以使用大部分GDI绘图函数在该MetaFile设备内容上进行绘图。   
这些GDI呼叫并不在任何具体的设备上绘图，相反地，它们被储存在MetaFile中。当关闭MetaFile设备内容时，会得到MetaFile的句柄。这时就可以在某个具体的设备内容上「播放」这个MetaFile，这与直接执行MetaFile中GDI函数的效果等同。   
CreateMetaFile只有一个参数，它可以是NULL或文件名称。如果是NULL，则MetaFile储存在内存中。如果是文件名称（以.WMF作为「Windows MetaFile」的扩展名），则MetaFile储存在磁盘文件中。  
程序MetaFile显示了在WM_CREATE消息处理期间建立内存MetaFile，并在WM_PAINT消息处理期间将图像显示100遍。   
参考MetaFile.c
### 将MetaFile储存在磁盘上
在上面的例子中，CreateMetaFile的NULL参数表示要建立储存在内存中的MetaFile。  
我们也可以建立作为文件储存在磁盘上的MetaFile，这种方法对于大的MetaFile比较合适，因为可以节省内存空间。但是，每次使用磁盘上的MetaFile时，就需要存取磁盘。  
要把MetaFile存在磁盘上，需要把CreateMetaFile的NULL参数替换为文件名称。在WM_CREATE处理结束时，仍然可以用MetaFile句柄来呼叫DeleteMetaFile，这样句柄被删除，但是磁盘文件仍然被储存着。  
在处理WM_PAINT消息处理期间，可以通过呼叫GetMetaFile来取得此磁盘文件的MetaFile句柄：  
```c
hmf = GetMetaFile (szFileName) ;  
```  
现在就可以像前面那样显示这个MetaFile。使用完成后，删除该MetaFile句柄：   
```c
DeleteMetaFile (hmf) ;   
```
如果需要删除磁盘文件，不用保存的话：   
```c
DeleteFile (szFileName) ;   
```   
正如在第十章讨论过的，**MetaFile也可以作为使用者自订资源**。您可以简单地把它当作数据块加载。如果您有一块包含MetaFile内容的资料，那么您可以使用   
```c
hmf = SetMetaFileBitsEx (iSize, pData) ;  
```   
来建立MetaFile。SetMetaFileBitsEx有一个对应的函数－GetMetaFileBitsEx，此函数将MetaFile的内容复制到内存块中。
### 老式MetaFile与剪贴簿
老式MetaFile有个缺陷。如果有老式MetaFile的句柄，无法在显示的时候确定它的大小，除非深入分析MetaFile的内部结构，否则无法得知。  
此外，当程序从剪贴簿取得老式MetaFile时，如果MetaFile被定义为在MM_ISOTROPIC或MM_ANISOTROPIC映像方式下显示，则此程序在使用该MetaFile时具有最大程度的灵活性。程序收到该MetaFile后，就可以在显示它之前简单地通过设定视埠的范围来缩放图像。然而，如果MetaFile内的映像方式被设定为MM_ISOTROPIC或MM_ANISOTROPIC，则收到该MetaFile的程序将无法继续执行。程序仅能在显示MetaFile之前或之后进行GDI呼叫，不允许在显示MetaFile当中进行GDI呼叫。   
为了解决这些问题，老式MetaFile句柄不直接放入剪贴簿供其它程序取得，而是作为「MetaFile图片」（MetaFilePICT结构型态）的一部分。此结构使得从剪贴簿上取得MetaFile图片的程序能够在显示MetaFile之前设定映像方式和视埠范围。   
MetaFilePICT结构的长度为16个字节，定义如下：   
```c
typedef struct tagMetaFilePICT   
{    
	LONG mm ; // mapping mode   
	LONG xExt ; // width of the MetaFile image   
	LONG yExt ; // height of the MetaFile image  
	LONG hMF ; // handle to the MetaFile  
}    
MetaFilePICT ;    
```   
**对于MM_ISOTROPIC和MM_ANISOTROPIC以外的所有映像方式**，图像大小用xExt和yExt值表示，其单位是由mm给出的映像方式的单位。利用这些信息，从剪贴簿复制MetaFile图片结构的程序就能够确定在显示MetaFile时所需的显示空间。建立该MetaFile的程序可以将这些值设定为输入MetaFile的GDI绘制函数中所使用的最大的x坐标和y坐标值。    
**在MM_ISOTROPIC和MM_ANISOTROPIC映射方式下**，xExt和yExt字段有不同的功能。我们在第五章中曾介绍过一个程序，该程序为了在GDI函数中使用与图像实际尺寸无关的逻辑单位而采用MM_ISOTROPIC或MM_ANISOTROPIC映射方式。当程序只想保持纵横比而可以忽略图形显示平面的大小时，采用MM_ISOTROPIC模式；反之，当不需要考虑纵横比时采用MM_ANISOTROPIC模式。您也许还记得，第五章中在程序将映像方式设定为MM_ISOTROPIC或MM_ANISOTROPIC后，通常会呼叫SetWindowExtEx和SetViewportExtEx。SetWindowExtEx呼叫使用逻辑单位来指定程序在绘制时使用的单位，而SetViewportExtEx呼叫使用的设备单位大小则取决于图形显示平面（例如，窗口显示区域的大小）。   
如果程序为剪贴簿建立了MM_ISOTROPIC或MM_ANISOTROPIC方式的MetaFile，则该MetaFile本身不应包含对SetViewportExtEx的呼叫，因为该呼叫中的设备单位应该依据建立MetaFile的程序的显示平面，而不是依据从剪贴簿读取并显示MetaFile的程序的显示平面。从剪贴簿取得MetaFile的程序可以利用xExt和yExt值来设定合适的视埠范围以便显示MetaFile。但是当映像方式是MM_ISOTROPIC或MM_ANISOTROPIC时，MetaFile本身包含设定窗口范围的呼叫。MetaFile内的GDI绘图函数的坐标依据这些窗口的范围。   
建立MetaFile和MetaFile图片遵循以下规则：   
1 设定MetaFilePICT结构的mm字段来指定映像方式。   
2 **对于MM_ISOTROPIC和MM_ANISOTROPIC以外的映像方式**，xExt与yExt字段设定为图像的宽和高，单位与mm字段相对应。对于在MM_ISOTROPIC或MM_ANISOTROPIC方式下显示的MetaFile，工作要复杂一些。**在MM_ANISOTROPIC模式下**，当程序既不对图片大小跟纵横比给出任何建议信息时，xExt和yExt的值均为零。在这两种模式下，如果xExt和yExt的值为正数，它们就是以0.01mm单位（MM_HIMETRIC单位）表示该图像的宽度和高度。**在MM_ISOTROPIC方式下**，如果xExt和yExt为负值，它们就指出了图像的纵横比而不是大小。   
3 **在MM_ISOTROPIC和MM_ANISOTROPIC映像方式下**，MetaFile本身含有对SetWindowExtEx的呼叫，也可能有对SetWindowOrgEx的呼叫。亦即，建立MetaFile的程序在MetaFile设备内容中呼叫这些函数。MetaFile一般不会包含对SetMapMode、SetViewportExtEx或SetViewportOrgEx的呼叫。   
4 MetaFile应该是内存MetaFile，而不是MetaFile文件。  
以下将有一小段范例代码：   
1 首先是创建并存储进剪贴簿的过程：   
```c  
//创建并设置原点   
hdcMeta = CreateMetaFile (NULL) ;   
SetWindowExtEx (hdcMeta, ...) ;   
SetWindowOrgEx (hdcMeta, ...) ;   
//关闭Meta并获得句柄   
hmf = CloseMetaFile (hdcMeta) ;    
//为MetaFilePICT结构配置内存   
GLOBALHANDLE hGlobal ;  
LPMetaFilePICT pMFP ;    
hGlobal= GlobalAlloc (GHND | GMEM_SHARE, sizeof (MetaFilePICT)) ;   
//固定内存以便输入内容  
pMFP = (LPMetaFilePICT) GlobalLock (hGlobal) ;   
pMFP->mm = MM_... ;   
pMFP->xExt = ... ;   
pMFP->yExt = ... ;   
pMFP->hMF = hmf ;   
GlobalUnlock (hGlobal) ;//输入后解锁内存句柄   
//然后，程序将包含有MetaFile图片的整体内存块传送给剪贴簿    
OpenClipboard (hwnd) ;   
EmptyClipboard () ;    
SetClipboardData (CF_MetaFilePICT, hGlobal) ;   
CloseClipboard () ;    
```   
2 从剪贴簿获取MetaFile并且使用：   
```c
//首先打开剪贴簿，得到MetaFile图片结构句柄并将其锁定   
OpenClipboard (hwnd) ;   
hGlobal = GetClipboardData (CF_MetaFilePICT) ;    
pMFP = (LPMetaFilePICT) GlobalLock (hGlobal) ;   
//存储当前设备内容属性，并将映像方式设定为结构中的mm值   
SaveDC (hdc) ;  
SetMappingMode (pMFP->mm) ;   
//这里对于映像方式是不是MM_ISOTROPIC或MM_ANISOTROPIC做一个判断，写到一个函数里   
void PrepareMetaFile ( HDC hdc, LPMetaFilePICT pmfp,
int cxClient, int cyClient)  
{    
	int xScale, yScale, iScale ;  
	SetMapMode (hdc, pmfp->mm) ;    
	if (pmfp->mm == MM_ISOTROPIC || pmfp->mm == MM_ANISOTROPIC)  
	{   
		if (pmfp->xExt == 0)   
			SetViewportExtEx (hdc, cxClient, cyClient, NULL) ;   
		else if (pmfp->xExt > 0)     
			SetViewportExtEx (hdc,
			pmfp->xExt * GetDeviceCaps (hdc, HORZRES) /
			GetDeviceCaps (hdc, HORZSIZE) / 100),
			pmfp->yExt * GetDeviceCaps (hdc, VERTRES)
			GetDeviceCaps (hdc, VERTSIZE) / 100), NULL) ;  
		else if (pmfp->xExt < 0)   
		{    
			xScale = 100 *cxClient * GetDeviceCaps (hdc, HORZSIZE) /
			GetDeviceCaps (hdc, HORZRES) / -pmfp->xExt ;  
			lScale = 100 *cyClient * GetDeviceCaps (hdc, VERTSIZE) /
			GetDeviceCaps (hdc, VERTRES) / -pmfp->yExt ;  
			iScale = min (xScale, yScale) ;  
			SetViewportExtEx (hdc, -pmfp->xExt * iScale * GetDeviceCaps (hdc, HORZRES) /
			GetDeviceCaps (hdc, HORZSIZE) / 100, -pmfp->yExt * iScale
			* GetDeviceCaps (hdc, VERTRES) / GetDeviceCaps (hdc, VERTSIZE) / 100, NULL) ;   
		}  
	}   
}    
//设定视埠原点，显示MetaFile，并恢复设备内容：  
PlayMetaFile (pMFP->hMF) ;  
RestoreDC (hdc, -1) ;    
//对内存块解锁并关闭剪贴簿：   
GlobalUnlock (hGlobal) ;   
CloseClipboard () ;    
``` 
**如果程序使用增强型MetaFile就可以省去这项工作。**当某个应用程序将这些格式放入剪贴簿而另一个程序却要求从剪贴簿中获得其它格式时，Windows剪贴簿会自动在老式MetaFile和增强型MetaFile之间进行格式转换。