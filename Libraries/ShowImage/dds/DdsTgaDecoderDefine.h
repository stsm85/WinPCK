//////////////////////////////////////////////////////////////////////
// miscpicdlg.h: WinPCK 界面线程部分
// 对话框代码，解码dds、tga的模块
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#ifndef _MISCPICDLG_H_
#define _MISCPICDLG_H_

//#define	FMT_RAW			0	//bmp,jpg,png,gif,emf.....
//#define	FMT_DDS			1
//#define	FMT_TGA			2

#define	FMTTGA_A1R5G5B5		16
#define	FMTTGA_R8G8B8		24
#define	FMTTGA_A8R8G8B8		32

#define	FMTTGA_RAWTBL	1
#define	FMTTGA_RAWRGB	2
#define	FMTTGA_RLETBL	9
#define	FMTTGA_RLERGB	10

#include <stdint.h>
typedef enum { FMT_DDS, FMT_TGA, FMT_RAW, FMT_UNKNOWN } PICFORMAT;

typedef unsigned __int64 QWORD, *LPQWORD;

#pragma pack(push)
#pragma pack(4)

typedef struct {
	uint8_t	b;
	uint8_t	g;
	uint8_t	r;
	uint8_t	a;
}CBRGB;

typedef union _CMYRGB{
	CBRGB cRGBVal;
	uint32_t dwargb;
}CMYRGB, *LPCRGB;

#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)

typedef struct _TGAHEAD {

	uint8_t		byteTgaInfoSize;		//图像信息字段（见本子表的后面）的字符数
									//本字段是 1 字节无符号整型，指出了图像格式区别字段长度
									//其取值范围是 0 到 255 ，当它为 0 时表示没有图像的
									//信息字段。
	uint8_t		byteColorTable;			//颜色表的类型
									//该字段为表示对应格式 1 的图像而包含一个二进制 1
	uint8_t		byteTgaType;			//图像类型码
									//该字段总为 1 ， 这也是此类型为格式 1 的原因
	//颜色表规格,如果颜色表类型字段为 0 则被忽略；否则描述如下
	uint16_t	wColorTableOffset;		//颜色表首址
	uint16_t	wColorTableSize;		//颜色表的长度
	uint8_t		byteColorTableBits;		//颜色表表项的位（bit）数
	//图像规格
	uint16_t	wXOffset;				//图像 x 坐标起始位置
	uint16_t	wYOffset;				//图像 y 坐标起始位置
	uint16_t	wWidth;					//图像宽度
	uint16_t	wHeight;				//图像高度
	uint8_t		byteBitsPerPixel;		//图像每像素存储占用位（bit）数
	//图像描述符字节
	uint8_t		byteAttrBitsPerPixel:4;	//bits 3-0 - 每像素对应的属性位的位数；
									//对于 TGA 16，该值为 0 或 1，
									//对于 TGA 24，该值为 0，
									//对于 TGA 32，该值为 8
	uint8_t		byteTgaOtherConfig:4;	//bit 4    - 保留，必须为 0
									//bit 5    - 屏幕起始位置标志
									//0 = 原点在左下角
									//1 = 原点在左上角
									//对于 truevision 图像必须为 0
									//bits 7-6 - 交叉数据存储标志
									//	00 = 无交叉
									//	01 = 两路奇/偶交叉
									//	10 = 四路交叉
									//	11 = 保留
} TGAHEAD, *LPTGAHEAD;

//typedef struct _BMPHEAD {
//
//	UINT16	wBmpHead;
//	UINT32	dwFilesize;
//	UINT32	dwReserved;
//	UINT32	dwBmpDataOffset;
//
//	UINT32	dwBmpHeaderSize;
//	UINT32	dwWidth;
//	UINT32	dwHeight;
//	UINT16	wPlanes;
//	UINT16	wBitsPerPixel;
//	UINT32	dwCompression;
//	UINT32	dwBitmapDataSize;
//	UINT32	dwhresolution;//ec4
//	UINT32	dwvresolution;//ec4
//	UINT32	dwColors;
//	UINT32	dwImportantColors;
//
//	//struct palette{
//	//	BYTE	blue;
//	//	BYTE	green;
//	//	BYTE	red;
//	//	BYTE	zero;
//	//};
//
//} BMPHEAD, *LPBMPHEAD;
//
//typedef struct _BMPHEAD {
//	BITMAPFILEHEADER filehead;
//	BITMAPV4HEADER	DIBhead;
//} BMPHEAD, *LPBMPHEAD;
//

//typedef struct tagBITMAPINFOHEADER{
//  DWORD  biSize; 
//  LONG   biWidth; 
//  LONG   biHeight; 
//  WORD   biPlanes; 
//  WORD   biBitCount; 
//  DWORD  biCompression; 
//  DWORD  biSizeImage; 
//  LONG   biXPelsPerMeter; 
//  LONG   biYPelsPerMeter; 
//  DWORD  biClrUsed; 
//  DWORD  biClrImportant; 
//} BITMAPINFOHEADER, *PBITMAPINFOHEADER; 

//BITMAPINFOHEADER
#pragma pack(pop)

#endif

