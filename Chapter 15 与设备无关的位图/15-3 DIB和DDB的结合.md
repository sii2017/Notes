## DIB和DDB的结合
在上一章，我们了解了使用GDI函数在DDB上绘制图像的方法。到目前为止，还没有在DIB上绘图的方法。   
**另一个问题是SetDIBitsToDevice和StretchDIBits没有BitBlt和StretchBlt速度快**，尤其在Windows NT环境下以及执行许多最接近颜色搜寻（例如，在8位视频卡上显示24位DIB）时。   
因此，在DIB和DDB之间进行转换是有好处的。例如，如果我们有一个需要在屏幕上显示许多次的DIB，那么把DIB转换为DDB就很有意义，这样我们就能够使用快速的BitBlt和StretchBlt函数来显示它了。   
### DIB建立DDB
从DIB中建立GDI位图对象，（从上一章中）基本上我们已经知道了方法：如果有DIB，就能够使用CreateCompatibleBitmap来建立与DIB大小相同并与视讯显示器兼容的GDI位图对象（空白位图对象）。然后将该位图对象选入内存设备内容并呼叫SetDIBitsToDevice在那个内存DC上绘图（将DIB画到内存设备内容里的空的位图对象上）。结果就是DDB具有与DIB相同的图像，但具有与视讯显示器兼容的颜色组织。（这是上一章对于DDB绘图的方法）    
在这一章中的DIB，也可以使用另一个方法：调用CreateDIBitmap用几个步骤完成上述工作。函数的语法为：   
```c
hBitmap = CreateDIBitmap (    
	hdc, // device context handle     
	pInfoHdr, // pointer to DIB information header   
	fInit, // 0 or CBM_INIT   
	pBits, // pointer to DIB pixel bits   
	pInfo, // pointer to DIB information  
	fClrUse) ; // color use flag     
```    
需要注意pInfoHdr和pInfo这两个参数，它们分别定义为指向BITMAPINFOHEADER结构和BITMAPINFO结构的指针。正如之前所说，BITMAPINFO结构是后面紧跟色彩对照表的BITMAPINFOHEADER结构。我们一会儿会看到这种区别所起的作用。而最后一个参数是DIB_RGB_COLORS（等于0）或DIB_PAL_COLORS，它们在SetDIBitsToDevice函数中使用。下一章将讨论更多这方面的内容。    
理解Windows中位图函数的作用是很重要的。不要考虑CreateDIBitmap函数的名称，它不建立与「设备无关的位图」，**它从设备无关的规格中建立「设备相关的位图」**。注意该函数传回GDI位图对象的句柄，CreateBitmap、CreateBitmapIndirect和CreateCompatibleBitmap也与它一样。   
调用CreateDIBitmap函数最简单的方法是：    
```c
hBitmap= CreateDIBitmap(NULL, pbmih, 0, NULL, NULL, 0);
```    
唯一的参数是指向BITMAPINFOHEADER结构（不带色彩对照表）的指标。在这个形式中，函数建立**单色GDI位图对象**。第二种简单的方法是：   
```c
hBitmap= CreateDIBitmap(hdc, pbmih, 0, NULL, NULL, 0);   
```   
在这个形式中，函数建立了与设备内容兼容的DDB，该设备内容由hdc参数指出。    
在之前的章节里，我们都是通过CreateBitmap（建立单色位图）或CreateCompatibleBitmap（建立与视讯显示器兼容的位图）来完成一些工作。   
在上面CreateDIBitmap的这两个简化模式中，**图素还未被初始化。如果CreateDIBitmap的第三个参数是CBM_INIT，Windows就会建立DDB并使用最后三个参数初始化位图位。**pInfo参数是指向**包括色彩对照表的BITMAPINFO结构的指针**。pBits参数是指向由**BITMAPINFO结构指出的色彩格式中的位数组的指针**，根据色彩对照表这些位被转换为设备的颜色格式，这与SetDIBitsToDevice的情况相同。   
如果仅需显示DIB一次，并担心SetDIBitsToDevice显示太慢，则调用CreateDIBitmap并使用BitBlt或StretchBlt来显示DDB就没有什么意义。因为SetDIBitsToDevice和CreateDIBitmap都执行颜色转换，这两个工作会占用同样长的时间。只有在多次显示DIB时（例如在处理WM_PAINT消息时）进行这种转换才有意义。    
参考DIBCONV.C    
本程序将DIB先放到堆里的内存中，然后将堆里的内存里的DIB转化成DDB，放到栈的变量位图句柄中，最后通过上一章学到的，将位图放入内存设备内容中，最后再画到hdc的画面上。   
在从CreateDIBitmap建立位图时不必初始化DDB图素位（如果第三个参数是CBM_INIT，Windows就会建立DDB并使用最后三个参数初始化位图位。反之则不初始化），之后可以呼叫SetDIBits初始化图素位。该函数的语法如下：    
```c
iLines = SetDIBits (    
	hdc, // device context handle   
	hBitmap, // bitmap handle  
	yScan, // first scan line to convert   
	cyScans, // number of scan lines to convert   
	pBits, // pointer to pixel bits   
	pInfo, // pointer to DIB information   
	fClrUse) ; // color use flag   
```   
函数使用了BITMAPINFO结构中的色彩对照表把位转换为设备相关的格式。**只有在最后一个参数设定为DIB_PAL_COLORS时，才需要设备内容句柄**。    
### 从DDB到DIB
与SetDIBits函数相似的函数是GetDIBits，可以使用此函数把DDB转化为DIB：  
```c   
int WINAPI GetDIBits (   
	hdc, // device context handle   
	hBitmap, // bitmap handle   
	yScan, // first scan line to convert    
	cyScans, // number of scan lines to convert   
	pBits, // pointer to pixel bits (out)   
	pInfo, // pointer to DIB information (out)   
	fClrUse) ; // color use flag    
```  
然而，此函数产生的恐怕不是SetDIBits的反运算结果。在一般情况下，如果使用CreateDIBitmap和SetDIBits将DIB转换为DDB，然后使用GetDIBits把DDB转换回DIB，并不会得到原来的图像。这是因为在DIB被转换为设备相关的格式时，有一些信息遗失了。遗失的信息数量取决于进行转换时Windows所执行的显示模式。  
您可能会发现没有使用GetDIBits的必要性。考虑一下：在什么环境下您的程序发现自身带有位图句柄，但没有用于在起始的位置建立位图的数据？剪贴簿？但是剪贴簿为DIB提供了自动的转换。**GetDIBits函数的一个例子是在捕捉屏幕显示内容的情况下**，例如第十四章中BLOWUP程序所做的。我不示范这个函数，但在Microsoft网站的Knowledge Base文章Q80080中有一些信息。      
### DIB区块
现在已经对设备相关和设备无关位图的区别有了清晰的概念。   
1 DIB能拥有几种色彩组织中的一种，DDB必须是单色的或是与真实输出设备相同的格式。   
2 DIB是一个文件或内存块，DDB是GDI位图对象并由位图句柄表示。    
3 DIB能被显示或转换为DDB并转换回DIB，但是这里包含了设备无关位和设备相关位之间的转换程序。    
现在有一个函数打破了这些规则。该函数在32位Windows版本中发表，称为CreateDIBSection，语法为：    
```c   
hBitmap = CreateDIBSection (    
	hdc, // device context handle   
	pInfo, // pointer to DIB information   
	fClrUse, // color use flag    
	ppBits, // pointer to pointer variable   
	hSection, // file-mapping object handle   
	dwOffset) ; // offset to bits in file-mapping object   
```   
CreateDIBSection是Windows API中最重要的函数之一（至少在使用位图时），然而您会发现它很深奥并难以理解。(...)   
CreateDIBSection所做的就是建立了DIB的一部分（位图图素位的内存块）。   
关于返回值，它是GDI位图对象的句柄，这个返回值可能是该函数调用中最会误会的部分。返回值似乎暗示着CreateDIBSection在功能上与CreateDIBitmap相同。事实上，它只是相似但完全不同。**实际上，从CreateDIBSection传回的位图句柄与我们在本章和上一章遇到的所有位图建立函数传回的位图句柄在本质上不同**。  
首先最简化的调用CreateDIBSection。首先，把最后两个参数hSection和dwOffset，分别设定为NULL和0，我们将在本章最后讨论这些参数的用法。第二，仅在fColorUse参数设定为DIB_PAL_COLORS时，才使用hdc参数，如果fColorUse为DIB_RGB_COLORS（或0），hdc将被忽略（这与CreateDIBitmap不同，hdc参数用于取得与DDB兼容的设备的色彩格式）。
因此，**CreateDIBSection最简单的形式仅需要第二和第四个参数**。第二个参数是指向BITMAPINFO结构的指针，以前曾使用过。而第四个参数实际上很简单。   
假设要建立每图素24位的384×256位DIB，24位格式不需要色彩对照表（一个字节8位，三个字节24位），因此它是最简单的，所以我们可以为BITMAPINFO参数使用BITMAPINFOHEADER结构。需要定义三个变量：BITMAPINFOHEADER结构、BYTE指针和位图句柄：  
```c
BITMAPINFOHEADER bmih ;
BYTE * pBits ;
HBITMAP hBitmap ;   
```   
然后初始化BITMAPINFOHEADER结构的字段   
```c 
bmih->biSize = sizeof (BITMAPINFOHEADER) ;   
bmih->biWidth = 384 ;   
bmih->biHeight = 256 ;  
bmih->biPlanes = 1 ;   
bmih->biBitCount = 24 ;   
bmih->biCompression = BI_RGB ;  
bmih->biSizeImage = 0 ;   
bmih->biXPelsPerMeter = 0 ;  
bmih->biYPelsPerMeter = 0 ;   
bmih->biClrUsed = 0 ;   
bmih->biClrImportant = 0 ;   
```   
在基本准备后，我们呼叫该函数：   
```c
hBitmap = CreateDIBSection (NULL, (BITMAPINFO *) &bmih, 0, &pBits, NULL, 0) ;  
```   
注意:我们为第二个参数赋予BITMAPINFOHEADER结构的地址。这是常见的，但一个BYIE指针pBits的地址，就不常见了。这样，第四个参数是函数需要的指向指标的指标。   
**这是函数调用所做的：CreateDIBSection检查BITMAPINFOHEADER结构并配置足够的内存块来加载DIB图素位。（在这个例子里，内存块的大小为384×256×3字节。）它在您提供的pBits参数中储存了指向此内存块的指针。函数传回位图句柄，正如我说的，它与CreateDIBitmap和其它位图建立函数传回的句柄不一样。**   
然而，我们还没有做完，位图图素是未初始化的。如果正在读取DIB文件，可以简单地把pBits参数传递给ReadFile函数并读取它们。或者可以使用一些程序代码「人工」设定。    
以下DIBSECT除了呼叫CreateDIBSection而不是CreateDIBitmap之外，与DIBCONV程序相似。    
参考DIBSECT.C   
注意DIBCONV中的CreateBitmapObjectFromDibFile函数和DIBSECT中的CreateDIbsectionFromDibFile函数之间的区别。   
DIBCONV读入整个文件，然后把指向DIB内存块的指针传递给CreateDIBitmap函数。    
DIBSECT首先读取BITMAPFILEHEADER结构中的信息，然后确定BITMAPINFO结构的大小，为此配置内存，并在第二个ReadFile呼叫中将它读入内存。然后，函数把指向BITMAPINFO结构和指针变量pBits的指针传递给CreateDIBSection。函数传回位图句柄并设定pBits指向函数将要读取DIB图素位的内存块。     
> 1 先将DIB内容读取到bmfh中，并获取pbmi。   
> 2 在函数CreateDIBSection中将pbmi作为参数给出，并且关联pBits指针。   
> 3 将DIB内容中的图素存入pBits指针中。   
> 4 这时候位图句柄就完整的返回出去。   

当在视讯显示器上显示DIB时，某些时候必须进行从设备无关图素到设备相关图素的转换，有时这些格式转换可能相当费时。来看一看三种用于显示DIB的方法：   
1 当使用SetDIBitsToDevice或StretchDIBits来把DIB直接显示在屏幕上，格式转换在SetDIBitsToDevice或StretchDIBits呼叫期间发生。   
2 当使用CreateDIBitmap和（可能是）SetDIBits把DIB转换为DDB，然后使用BitBlt或StretchBlt来显示它时，如果设定了CBM_INIT旗标，格式转换在CreateDIBitmap或SetDIBits期间发生。    
3 当使用CreateDIBSection建立DIB区块，然后使用BitBlt或StretchBlt显示它时，格式转换在BitBlt对StretchBlt的呼叫期间发生。   
ps：个人理解，如果有初始化则在初始化的时候转化如2，如果没有则在调用显示函数的时候进行转换。    
这是从CreateDIBSection传回的位图句柄不同于我们所遇到的其它位图句柄的一个地方。**此位图句柄实际上指向储存在内存中由系统维护但应用程序能存取的DIB。**在需要的时候，DIB会转化为特定的色彩格式，通常是在用BitBlt或StretchBlt显示位图时。
### DIB区块的其它区别
从CreateDIBitmap传回的位图句柄与函数的hdc参数引用的设备有相同的平面和图素字节。
### 文件映像选项
### 总结