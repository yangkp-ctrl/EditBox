/*--------------------------------------------------------------------------------------------------
// File : miniword.h
//
// Description:
//             包含主控模块所使用的结构体的声明
//
// Author: 程鑫
//
// Date: 2018-04-19
//
--------------------------------------------------------------------------------------------------*/

#pragma once

#include <Windows.h>
#include <shellapi.h>

#include "resource.h"

#define MAX_LOADSTRING 100

////////////////////////// 定时器模块 ///////////////////////////////
typedef struct _TIMER
{
public:
	_TIMER(const DWORD& tr);
	~_TIMER();

	DWORD     time_rest;       // 定时器时长
	HANDLE    time_mutex;      // 互斥锁做事件触发
	HANDLE    time_thread_id;  // 定时器运行的子线程id
} TIMER, *HTIMER;

/* ------------ struct TIMER's function statement ----------- */
HTIMER _stdcall CreateTimer(DWORD time_length);
BOOL   _stdcall KillTimer(HTIMER timer);
BOOL            IsTimerStopped(HTIMER hTimer);
/* ------------------------ end ----------------------------- */

////////////////////////// 内核模块 /////////////////////////////////
class CText;
typedef CText *HTEXT, *LPTEXT;

// 文本用户结构
typedef struct _TEXT_USER
{
	HFONT m_hFont;			  // 字体实例
	POINT m_pCharPixelSize;   // 字符像素大小
	POINT m_pCaretPixelSize;  // 光标像素大小
	POINT m_pCaretPixelPos;   // 光标像素位置
	COORD m_cCaretCoord;      // 光标逻辑坐标(X列 Y行)

	POINT m_pMinCharPixelSize;  // 最小字符像素大小
	POINT m_pMaxCharPixelSize;  // 最大字符像素大小

	ULONGLONG m_fMask;                     // 查找模式
	TCHAR m_szFindWhat[MAX_LOADSTRING];    // 查找串
	TCHAR m_szReplaceWhat[MAX_LOADSTRING]; // 替换串
} TEXTUSER, *LPTEXTUSER, *HTEXTUSER;
/* ----------- struct TEXTUSER's function statement ------------ */
HTEXTUSER __stdcall CreateUser(HWND hWnd);
BOOL      __stdcall ReleaseUser(HWND hWnd, HTEXTUSER hUser);
/* --------------------------- end ----------------------------- */

// 文本内核结构
typedef struct _TEXT_KERNEL
{
	HTEXT m_hText;          // 文本对象实例
	POINT m_pTextPixelSize; // 文本像素大小
	POINT m_pStartPixelPos; // 起点像素位置
	POINT m_pEndPixelPos;   // 终点像素位置
} TEXTKERNEL, *LPTEXTKERNEL, *HTEXTKERNEL;
/* ----------- struct TEXTKERNEL's function statement ------------- */
HTEXTKERNEL __stdcall CreateKernel(HWND hWnd, HTEXTUSER hUser);
BOOL        __stdcall ReleaseKernel(HWND hWnd, HTEXTKERNEL hKernel);
/* ----------------------------- end ------------------------------ */

/* 文本图形设备结构 */
typedef struct _TEXT_GDI
{
	POINT   m_pClientPixelSize; // 显示区像素大小
	POINT   m_pPaintPixelPos;   // 绘图区像素位置
	POINT   m_pPaintPixelSize;  // 绘图区像素大小
	POINT   m_pPageSize;        // 页面范围(X列 Y行)

	HWND    m_hStatus;          // 状态栏窗口
	POINT   m_pBufferPixelSize; // 缓冲位图像素大小
	HDC     m_hMemDC;           // 缓冲设备
	HBRUSH  m_hBrush;           // 默认背景刷
	HBITMAP m_hBitmap;          // 缓冲位图

	HTIMER  m_hMouseTimer;      // 鼠标移动定时器
	HTIMER  m_hSaveTimer;       // 自动保存定时器
} TEXTGDI, *LPTEXTGDI, *HTEXTGDI;
/* -------------- struct TEXTGDI's function statement ------------------- */
HTEXTGDI __stdcall CreateGDI(HWND hWnd, HINSTANCE hInst, HTEXTUSER hUser);
BOOL     __stdcall ReleaseGDI(HWND hWnd, HTEXTGDI hGDI);
/* -------------------------------- end --------------------------------- */

// 文本结构
typedef struct _TEXT_INFO
{
	HTEXTKERNEL	m_hKernel;  // 内核管理句柄
	HTEXTGDI	m_hGDI;     // 图形设备管理句柄
	HTEXTUSER	m_hUser;    // 用户管理句柄
	HWND		m_hWnd;     // 主窗口句柄
	HINSTANCE   m_hInst;    // 当前应用实例
} TEXTINFO, *LPTEXTINFO, *HTEXTINFO;
/* ------------- struct TEXTINFO's function statement ------------- */
HTEXTINFO __stdcall CreateTextInfo(HWND hWnd, HINSTANCE hInst);
BOOL      __stdcall ReleaseTextInfo(HWND hWnd, HTEXTINFO hTextInfo);
/* ---------------------------- end ------------------------------- */

//////////////////////////// IO模块 /////////////////////////////////
BOOL WINAPI MyTextOutW(_In_ HDC hdc, 
	_In_ int x, _In_ int y, _In_reads_(c) LPCWSTR lpString, 
	_In_ int c, _In_ short s, _In_ short e, _In_ int width);

#ifdef UNICODE
#define MyTextOut MyTextOutW
#else
#define MyTextOut MyTextOutA
#endif // UNICODE

//////////////////////////// 绘图模块 ///////////////////////////////
BOOL	MyInvalidateRect(HTEXTINFO, 
	LONG, LONG, LONG, LONG); // 设置无效区域

BOOL    AdjustCaretPosBeforeBackspace(HTEXTINFO); // 用户按Backspace键时 调整光标位置
BOOL    AdjustPaintPos(HTEXTINFO);                // 滑动窗口
BOOL    MoveCaret(HTEXTINFO, UINT);               // 移动光标

BOOL    SelectPaintPos(HTEXTINFO, POINT);         // 设置窗口位置
BOOL    SelectCaretPos(HTEXTINFO, POINT, COORD);         // 设置光标位置
BOOL    SelectHighlight(HTEXTINFO, POINT, POINT); // 设置高亮部分
BOOL    SelectTextSize(HTEXTINFO, POINT);         // 设置文本范围
BOOL    SelectWindow(HTEXTINFO, POINT, LPCWSTR);  // 命名文本窗口显示

BOOL    MyScrollWindow(HTEXTINFO, int, int);      // 滑动窗口

BOOL    PaintWindow(HTEXTINFO, LPRECT);           // 重绘窗口

BOOL    SelectClientSize(HTEXTINFO, LPRECT);      // 设置显示区大小(WM_SIZE)
BOOL    SelectCharSize(HTEXTINFO, LONG, LONG);    // 设置字体大小

// Convenient macro
// +++++++++++++++++++++++++++ Kernel ++++++++++++++++++++++++++ //
#define HTEXT(hTextInfo)    (hTextInfo->m_hKernel->m_hText)
#define TEXTSIZE(hTextInfo) (hTextInfo->m_hKernel->m_pTextPixelSize)
#define STARTPOS(hTextInfo) (hTextInfo->m_hKernel->m_pStartPixelPos)
#define ENDPOS(hTextInfo)   (hTextInfo->m_hKernel->m_pEndPixelPos)
// +++++++++++++++++++++++++++   End  ++++++++++++++++++++++++++ //

// +++++++++++++++++++++++++++   GDI  ++++++++++++++++++++++++++ //
#define PAINTSIZE(hTextInfo)  (hTextInfo->m_hGDI->m_pPaintPixelSize)
#define PAINTPOS(hTextInfo)   (hTextInfo->m_hGDI->m_pPaintPixelPos)
#define CLIENTSIZE(hTextInfo) (hTextInfo->m_hGDI->m_pClientPixelSize)
#define PAGESIZE(hTextInfo)   (hTextInfo->m_hGDI->m_pPageSize)
#define BUFFERSIZE(hTextInfo) (hTextInfo->m_hGDI->m_pBufferPixelSize)
#define MEMDC(hTextInfo)      (hTextInfo->m_hGDI->m_hMemDC)
#define BITMAP(hTextInfo)     (hTextInfo->m_hGDI->m_hBitmap)
#define BRUSH(hTextInfo)      (hTextInfo->m_hGDI->m_hBrush)
#define MTIMER(hTextInfo)     (hTextInfo->m_hGDI->m_hMouseTimer)
#define STIMER(hTextInfo)     (hTextInfo->m_hGDI->m_hSaveTimer)
#define STATUS(hTextInfo)     (hTextInfo->m_hGDI->m_hStatus)
// +++++++++++++++++++++++++++   End  ++++++++++++++++++++++++++ //

// +++++++++++++++++++++++++++   User ++++++++++++++++++++++++++ //
#define FONT(hTextInfo)          (hTextInfo->m_hUser->m_hFont)
#define CARETPOS(hTextInfo)      (hTextInfo->m_hUser->m_pCaretPixelPos)
#define CHARSIZE(hTextInfo)      (hTextInfo->m_hUser->m_pCharPixelSize)
#define CARETSIZE(hTextInfo)     (hTextInfo->m_hUser->m_pCaretPixelSize)
#define CARETCOORD(hTextInfo)    (hTextInfo->m_hUser->m_cCaretCoord)
#define MINCHARSIZE(hTextInfo)   (hTextInfo->m_hUser->m_pMinCharPixelSize)
#define MAXCHARSIZE(hTextInfo)   (hTextInfo->m_hUser->m_pMaxCharPixelSize)
#define MASK(hTextInfo)          (hTextInfo->m_hUser->m_fMask)
#define FINDSTRING(hTextInfo)    (hTextInfo->m_hUser->m_szFindWhat)
#define REPLACESTRING(hTextInfo) (hTextInfo->m_hUser->m_szReplaceWhat)
// +++++++++++++++++++++++++++   End  ++++++++++++++++++++++++++ //

// ++++++++++++++ Convenient Operation ++++++++++++++ //
BOOL operator==(POINT left, POINT right);

#define INRANGEX(x, l, r) ((x) >= (l) && (x) <= (r))
#define INRANGEY(y, t, b) ((y) >= (t) && (y) <  (b))

#define ZHWIDTH(l)  ((l) << 1)
#define USWIDTH(l)  (l)
#define ZHHEIGHT(l) (l)
#define USHEIGHT(l) (l)
// ++++++++++++++++++++++ END +++++++++++++++++++++++ //

// +++++++++++ FIND MACRO +++++++++++ //
#define DEFAULT 0x0000000000000000
#define FIND    0x0010000000000000
#define REPLACE 0x0001000000000000
// +++++++++++++++ END ++++++++++++++ //

// +++++++++++++++++++ CONFIG ++++++++++++++++++++++ //
#define SUB_KEY_VALUE      TEXT("*\\shell\\MiniWord")
#define DEFAULT_PATH       TEXT("C:\\MiniWord\\")
#define HELP_DOCUMENT_PATH TEXT("C:\\MiniWord\\Help\\readme.txt")
#define HELP_DOCUMENT_DIR  TEXT("C:\\MiniWord\\Help\\")
#define ICON_NAME          TEXT("miniword.ico")
#define DOCUMENT_LENGTH    2909
// +++++++++++++++++++++ END +++++++++++++++++++++++ //