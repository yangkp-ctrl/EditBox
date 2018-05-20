/*--------------------------------------------------------------------------------------------------
// File : *.cpp, *.h
// 
// Description:
//             (此处添加注释)
// 
// Author: XXX
// 
// Date: 2018-XX-XX 
//
--------------------------------------------------------------------------------------------------*/

#include <algorithm>
#include <cstring>

#include "stdafx.h"
#include "miniword.h"
#include "debug.h"

#define MAX_LOADSTRING 100

// 全局变量: 
static HINSTANCE hInst;                             // 当前实例
static TCHAR     szTitle[MAX_LOADSTRING];           // 标题栏文本
static TCHAR     szWindowClass[MAX_LOADSTRING];     // 主窗口类名
static HTEXTINFO hTextInfo;							// 单个文本信息管理实例
static TCHAR     szOpenFilePath[MAX_PATH];          // 待打开文件路径
static TCHAR     szSaveFilePath[MAX_PATH];          // 待保存文件路径

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Find(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);

	ZeroMemory(szOpenFilePath, MAX_PATH);
	if (nCmdShow > 0)
		lstrcpyn(szOpenFilePath, lpCmdLine, lstrlen(lpCmdLine) + 1);

	// 添加右键启动
	TCHAR szExePath[MAX_PATH];
	ZeroMemory(szExePath, _countof(szExePath));
	GetModuleFileName(NULL, szExePath, _countof(szExePath));

	HKEY  hRootKey = HKEY_CLASSES_ROOT;
	TCHAR szSubKey[MAX_PATH]    = SUB_KEY_VALUE;
	TCHAR szValueName[MAX_PATH] = TEXT("Icon");
	HKEY  hKeyResult;

	if (RegCreateKeyEx(hRootKey, szSubKey, 0, NULL, REG_OPTION_VOLATILE,
		KEY_ALL_ACCESS, NULL, &hKeyResult, NULL) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	TCHAR szSetValue[MAX_PATH];
	ZeroMemory(szSetValue, _countof(szSetValue));
	lstrcpyn(szSetValue, TEXT("Open w&ith MiniWord"), lstrlen(TEXT("Open w&ith MiniWord")) + 1);
	
	if (RegSetValueEx(hKeyResult, NULL, 0, REG_EXPAND_SZ, 
		(LPBYTE)szSetValue, sizeof(TCHAR) * lstrlen(szSetValue)) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	ZeroMemory(szSetValue, _countof(szSetValue));
	lstrcpyn(szSetValue, szExePath, lstrlen(szExePath) + 1);
	lstrcat(szSetValue, TEXT(" %1"));

	if (RegSetValueEx(hKeyResult, szValueName, 0, REG_EXPAND_SZ,
		(LPBYTE)szSetValue, sizeof(TCHAR) * lstrlen(szSetValue)) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lstrcat(szSubKey, TEXT("\\command"));
	if (RegCreateKeyEx(hRootKey, szSubKey, 0, NULL, REG_OPTION_VOLATILE,
		KEY_ALL_ACCESS, NULL, &hKeyResult, NULL) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (RegSetValueEx(hKeyResult, NULL, 0, REG_EXPAND_SZ,
		(LPBYTE)szSetValue, sizeof(TCHAR) * lstrlen(szSetValue)) != ERROR_SUCCESS)
	{
		return FALSE;
	}
	RegFlushKey(hKeyResult);
	RegCloseKey(hKeyResult);

	// 添加默认路径
	DWORD dwAttribute = GetFileAttributes(DEFAULT_PATH);
	if (dwAttribute == INVALID_FILE_ATTRIBUTES || !(dwAttribute & FILE_ATTRIBUTE_DIRECTORY))
		CreateDirectory(DEFAULT_PATH, NULL);
	ZeroMemory(szSaveFilePath, MAX_PATH);

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_EDITBOX, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EDITBOX));

	MSG msg;

	// 主消息循环: 
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style         = NULL;
	wcex.lpfnWndProc   = WndProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = hInstance;
	wcex.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EDITBOX));
	wcex.hCursor       = LoadCursor(nullptr, IDC_IBEAM);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName  = MAKEINTRESOURCEW(IDC_EDITBOX);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm       = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	HWND hWnd = CreateWindowW(szWindowClass, szTitle,
		WS_CAPTION |		// 标题栏
		WS_SYSMENU |		// 窗口调整按钮
		WS_HSCROLL |		// 水平滚动条
		WS_MINIMIZEBOX |	// 最小化按钮
		WS_VSCROLL		    // 竖直滚动条
		,
		0, 0, 743, 679, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//	WM_CREATE			- 创建窗口时初始化
//	WM_SHOWWINDOW		- 清空窗口（文本也被清理）
//  WM_COMMAND			- 处理应用程序菜单
//	WN_CLOSE			- 处理应用程序关闭时的资源释放
//	WM_SIZE				- 用户调整大小时进行重绘处理
//	WM_SETFOCOUS		- 用户重新单击应用程序
//	WM_KILLFOCUS		- 用户放弃应用程序
//	WM_KEYDOWN			- 处理虚拟按键
//	WM_CHAR				- 处理字符按键
//	WM_VSCROLL			- 处理用户拖动竖直滚动条
//	WM_HSCROLL			- 处理用户拖动水平滚动条
//	WM_LBUTTONDOWM		- 处理用户单击鼠标左键
//	WM_NCLBUTTONDBLCLK	- 处理用户双击鼠标左键击中标题栏
//	WM_LBUTTONDBLCLK	- 处理用户双击鼠标左键击中显示区
//	WM_MOUSEMOVE		- 处理用户滑动鼠标
//  WM_PAINT			- 绘制主窗口
//  WM_DESTROY			- 发送退出消息并返回
//	
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:	// 创建窗口
	{
#ifdef DEBUG
		assert(AllocConsole());
		FILE *stream;
		freopen_s(&stream, "CONIN$", "r+t", stdin);
		freopen_s(&stream, "CONOUT$", "w+t", stdout);

		wprintf_s(TEXT("Window created.\n"));
#endif // DEBUG

		HWND   hDesk;
		RECT   rc;
		hDesk = GetDesktopWindow();
		GetWindowRect(hDesk, &rc);
		//SetWindowLong(hWnd, GWL_STYLE, WS_CAPTION);
		//SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, rc.right, rc.bottom, SWP_SHOWWINDOW);

		hTextInfo = CreateTextInfo(hWnd, hInst);

		KERNELINFO kernelinfo;
		if (lstrcmp(szOpenFilePath, TEXT("")) != 0)
		{
			switch (UserMessageProc(HTEXT(hTextInfo),
				NULL, NULL, UM_OPEN, NULL, (SPARAM)szOpenFilePath, &kernelinfo))
			{
			case UR_SUCCESS:
				SelectWindow(hTextInfo, kernelinfo.m_pTextPixelSize, szOpenFilePath);
				break;
			default:
				break;
			}
		}
		else
		{
			switch (UserMessageProc(HTEXT(hTextInfo),
				NULL, NULL, UM_NEW, NULL, (SPARAM)DEFAULT_PATH, &kernelinfo))
			{
			case UR_SUCCESS:
				SelectWindow(hTextInfo, POINT{ 0, 0 }, kernelinfo.m_lpchText); // 默认文本窗口显示
				break;
			default:
				break;
			}
		}
	}
	break;
	case WM_SHOWWINDOW:	// 显示窗口
	{

	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_NEW:
		{	// 新建文件
			switch (UserMessageProc(HTEXT(hTextInfo),
				NULL, NULL, UM_ISSAVED, NULL, NULL, NULL))
			{
			case UR_NOTSAVED:
			{	// 用户未保存当前文件
				if (MessageBox(hWnd, TEXT("需要保存当前文件吗？"), TEXT("提示"), MB_YESNO) == IDYES)
					SendMessage(hWnd, WM_COMMAND, IDM_SAVE, NULL);	// 提示用户保存
			}
			case UR_SAVED:
			{	// 用户已保存当前文件
				KERNELINFO kernelinfo;
				switch (UserMessageProc(HTEXT(hTextInfo),
					NULL, NULL, UM_NEW, NULL, (SPARAM)DEFAULT_PATH, &kernelinfo))
				{
				case UR_SUCCESS:
					SelectWindow(hTextInfo, POINT{ 0, 0 }, kernelinfo.m_lpchText); // 默认文本窗口显示
					break;
				default:
					break;
				}
			}
			break;
			default:
				break;
			}
		}
		break;
		case IDM_OPEN:
		{	// 打开文件
			switch (UserMessageProc(HTEXT(hTextInfo),
				NULL, NULL, UM_ISSAVED, NULL, NULL, NULL))
			{
			case UR_NOTSAVED:
			{	// 用户未保存当前文件
				if (MessageBox(hWnd, TEXT("需要保存当前文件吗？"), TEXT("提示"), MB_YESNO) == IDYES)
					SendMessage(hWnd, WM_COMMAND, IDM_SAVE, NULL);	// 提示用户保存
			}
			case UR_SAVED:
			{	// 用户已保存当前文件
				if (lstrcmp(szOpenFilePath, TEXT("")) == 0)
				{
					OPENFILENAME ofn;
					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.hwndOwner = hWnd;
					ofn.hInstance = NULL;
					ofn.lpstrFilter = TEXT("*.txt");
					ofn.nFilterIndex = 1;
					ofn.lpstrCustomFilter = NULL;
					ofn.nMaxCustFilter = 0;
					ofn.lpstrFile = szOpenFilePath;
					ofn.nMaxFile = MAX_PATH;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = DEFAULT_PATH;
					ofn.lpstrTitle = NULL;
					ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_EXTENSIONDIFFERENT;
					ofn.nFileOffset = 0;
					ofn.nFileExtension = 0;
					ofn.lpstrDefExt = TEXT("txt");
					ofn.lCustData = 0L;
					ofn.lpfnHook = NULL;
					ofn.lpTemplateName = NULL;
					GetOpenFileName(&ofn);
				}
				if (lstrcmp(szOpenFilePath, TEXT("")) != 0)
				{	// 文件名合法
					KERNELINFO kernelinfo;
					switch (UserMessageProc(HTEXT(hTextInfo),
						NULL, NULL, UM_ISSAVED, NULL, (SPARAM)szOpenFilePath, &kernelinfo))
					{
					case UR_NOTSAVED: // 用户未保存当前文件
						SendMessage(hWnd, WM_COMMAND, IDM_SAVE, NULL);	// 提示用户保存当前文件
						break;
					default:
						break;
					}

					switch (UserMessageProc(HTEXT(hTextInfo),
						NULL, NULL, UM_OPEN, NULL, (SPARAM)szOpenFilePath, &kernelinfo))
					{
					case UR_SUCCESS:
						SelectWindow(hTextInfo, kernelinfo.m_pTextPixelSize, szOpenFilePath);
						break;
					default:
						break;
					}
				}
				ZeroMemory(szOpenFilePath, MAX_PATH);
			}
			break;
			default:
				break;
			}
		}
		break;
		case IDM_SAVE:
		{	// 保存文件
			KERNELINFO kernelinfo;
			switch (UserMessageProc(HTEXT(hTextInfo),
				NULL, NULL, UM_SAVE, NULL, NULL, &kernelinfo))
			{
			case UR_DEFAULTPATH:
			{
				ZeroMemory(szSaveFilePath, MAX_PATH);
				lstrcpyn(szSaveFilePath, DEFAULT_PATH, _countof(DEFAULT_PATH));
				GetWindowText(hWnd, szSaveFilePath + _countof(DEFAULT_PATH), MAX_PATH);

				TCHAR szAsk[STRING_SIZE];
				wsprintf(szAsk, TEXT("确认保存当前文件为：%ls?"), szSaveFilePath);
				switch (MessageBox(hWnd, szAsk, TEXT("提示"), MB_YESNOCANCEL))
				{
				case IDYES:
					UserMessageProc(HTEXT(hTextInfo),
						NULL, NULL, UM_SAVE, NULL, (SPARAM)szSaveFilePath, NULL);
				break;
				case IDNO:
					SendMessage(hWnd, WM_COMMAND, IDM_SAVEAS, NULL);
					break;
				default:
					break;
				}
			}
			break;
			default:
				break;
			}
		}
		break;
		case IDM_SAVEAS:
		{	// 另存为
			ZeroMemory(szSaveFilePath, MAX_PATH);
			OPENFILENAME ofn;
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd;
			ofn.hInstance = NULL;
			ofn.lpstrFilter = TEXT("*.txt");
			ofn.nFilterIndex = 1;
			ofn.lpstrCustomFilter = NULL;
			ofn.nMaxCustFilter = 0;
			ofn.lpstrFile = szSaveFilePath;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = DEFAULT_PATH;
			ofn.lpstrTitle = NULL;
			ofn.Flags = OFN_HIDEREADONLY | OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_EXTENSIONDIFFERENT;
			ofn.nFileOffset = 0;
			ofn.nFileExtension = 0;
			ofn.lpstrDefExt = TEXT("txt");
			ofn.lCustData = 0L;
			ofn.lpfnHook = NULL;
			ofn.lpTemplateName = NULL;

			if (GetSaveFileName(&ofn) && lstrcmp(szSaveFilePath, TEXT("")) != 0)
			{
				switch (UserMessageProc(HTEXT(hTextInfo),
					NULL, NULL, UM_SAVE, NULL, (SPARAM)szSaveFilePath, NULL))
				{
				case UR_SUCCESS:
					break;
				default:
					break;
				}
			}
		}
		break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;
		case IDM_FIND:
		{
			MASK(hTextInfo) &= DEFAULT;
			MASK(hTextInfo) |= FIND;
			DialogBox(hInst, MAKEINTRESOURCE(IDD_FINDDIALOG), hWnd, Find);
		}
		break;
		case IDM_REPLACE:
		{
			MASK(hTextInfo) &= DEFAULT;
			MASK(hTextInfo) |= REPLACE;
			DialogBox(hInst, MAKEINTRESOURCE(IDD_FINDDIALOG), hWnd, Find);
		}
		break;
		case IDM_CUT:
		{
			BYTE keyState[256] = { 0 };
			keyState[VK_CONTROL] = -1;
			SetKeyboardState(keyState);

			SendMessage(hWnd, WM_COMMAND, IDM_COPY, NULL);
			SendMessage(hWnd, WM_CHAR, TEXT('\b'), NULL);

			// 恢复Ctrl按键
			keyState[VK_CONTROL] = 0;
			SetKeyboardState(keyState);
		}
		break;
		case IDM_COPY:
		{
			if (OpenClipboard(hWnd))
			{	// 打开剪切板
				if (EmptyClipboard())
				{	// 清空剪切板
					KERNELINFO kernelinfo;
					switch (UserMessageProc(HTEXT(hTextInfo),
						NULL, NULL, UM_COPY, NULL, NULL, &kernelinfo))
					{
					case UR_SUCCESS:
					{
						SIZE_T   nSize    = kernelinfo.m_uiCount + 1;
						HGLOBAL  hMem     = GlobalAlloc(GHND, nSize * 2);	// UNICODE
						LPCWSTR  lpString = kernelinfo.m_lpchText;	// 获取拷贝串(不负责释放资源 只获取地址)
						if (hMem != NULL)
						{
							LPWSTR   lpchText = (LPWSTR)GlobalLock(hMem);
							lstrcpyn(lpchText, lpString, (int)nSize);
							SetClipboardData(CF_UNICODETEXT, hMem);
						}
					}
					break;
					default:
						break;
					}
				}
				CloseClipboard();
			}
		}
		break;
		case IDM_PASTE:
		{
			if (IsClipboardFormatAvailable(CF_UNICODETEXT))
			{	// UNICODE数据
				if (OpenClipboard(hWnd))
				{	// 从剪切板拷贝
					HGLOBAL hClipMemory = GetClipboardData(CF_UNICODETEXT);
					if (hClipMemory != NULL)
					{
						SIZE_T nSize        = GlobalSize(hClipMemory) / 2;
						LPWSTR lpClipMemory = (LPWSTR)GlobalLock(hClipMemory);
						LPWSTR lpchText     = new TCHAR[nSize + 1]{ 0 };

						lstrcpyn(lpchText, lpClipMemory, (int)nSize); // 粘贴

						GlobalUnlock(hClipMemory);
						CloseClipboard(); // 关闭剪切板

						KERNELINFO kernelinfo;
						switch (UserMessageProc(HTEXT(hTextInfo),
							CARETPOS(hTextInfo).x, CARETPOS(hTextInfo).y,
							UM_PASTE,
							NULL, (SPARAM)lpchText, &kernelinfo))
						{
						case UR_SUCCESS:
						{
							SelectTextSize(hTextInfo, kernelinfo.m_pTextPixelSize); // 设置文本像素大小

							MyInvalidateRect(hTextInfo,
								0, PAINTSIZE(hTextInfo).x,
								STARTPOS(hTextInfo).y - PAINTPOS(hTextInfo).y,
								ENDPOS(hTextInfo).y - PAINTPOS(hTextInfo).y + CHARSIZE(hTextInfo).y
							); // 设定粘贴之前的高亮区域为无效区域

							SelectHighlight(hTextInfo,
								kernelinfo.m_pStartPixelPos, kernelinfo.m_pEndPixelPos
							); // 设置高亮部分

							POINT pOldCaretPixelPos = CARETPOS(hTextInfo);
							SelectCaretPos(hTextInfo, kernelinfo.m_pCaretPixelPos, kernelinfo.m_cCaretCoord); // 设置光标在高亮区域的尾部

							MyInvalidateRect(hTextInfo,
								0, PAINTSIZE(hTextInfo).x,
								pOldCaretPixelPos.y - PAINTPOS(hTextInfo).y,
								CARETPOS(hTextInfo).y - PAINTPOS(hTextInfo).y + CHARSIZE(hTextInfo).y
							); // 设定粘贴之后的高亮区域为无效区域

							AdjustPaintPos(hTextInfo); // 滑动显示区 使光标落在显示区内部
						}
						break;
						default:
							break;
						}
						delete[] lpchText;	// 释放临时内存
					}
					CloseClipboard();
				}
			}
		}
		break;
		case IDM_CANCEL:
		{
			KERNELINFO kernelinfo;
			switch (UserMessageProc(HTEXT(hTextInfo), NULL, NULL, UM_CANCEL,
				NULL, NULL, &kernelinfo))
			{
			case UR_SUCCESS:
			{
				SelectTextSize(hTextInfo, kernelinfo.m_pTextPixelSize); // 设置文本像素大小

				MyInvalidateRect(hTextInfo,
					0, PAINTSIZE(hTextInfo).x,
					STARTPOS(hTextInfo).y - PAINTPOS(hTextInfo).y,
					ENDPOS(hTextInfo).y - PAINTPOS(hTextInfo).y + CHARSIZE(hTextInfo).y
				); // 设定撤销之前的高亮区域为无效区域

				SelectHighlight(hTextInfo,
					kernelinfo.m_pStartPixelPos, kernelinfo.m_pEndPixelPos
				); // 设置高亮部分

				MyInvalidateRect(hTextInfo,
					0, PAINTSIZE(hTextInfo).x,
					STARTPOS(hTextInfo).y - PAINTPOS(hTextInfo).y,
					ENDPOS(hTextInfo).y - PAINTPOS(hTextInfo).y + CHARSIZE(hTextInfo).y
				); // 设定撤销之后的高亮区域为无效区域

				if (kernelinfo.m_bLineBreak) // 添加换行符
					MyInvalidateRect(hTextInfo,
						0, PAINTSIZE(hTextInfo).x,
						CARETPOS(hTextInfo).y - PAINTPOS(hTextInfo).y - CHARSIZE(hTextInfo).y,
						PAINTSIZE(hTextInfo).y
					); // 设置从光标位置开始往下都为无效区域

				SelectCaretPos(hTextInfo, kernelinfo.m_pCaretPixelPos, kernelinfo.m_cCaretCoord); // 设置光标在高亮区域的尾部

				AdjustPaintPos(hTextInfo); // 滑动显示区 使光标落在显示区内部
			}
			break;
			default:
				break;
			}
		}
		break;
		case IDM_ALL:
		{
			KERNELINFO kernelinfo;
			switch (UserMessageProc(HTEXT(hTextInfo),
				NULL, NULL, UM_ALL, NULL, NULL, &kernelinfo))
			{
			case UR_SUCCESS:
			{
				SelectCaretPos(hTextInfo, kernelinfo.m_pEndPixelPos, kernelinfo.m_cCaretCoord); // 设置光标在高亮部分的尾部
				SelectHighlight(hTextInfo,
					STARTPOS(hTextInfo), ENDPOS(hTextInfo)
				); // 设置高亮部分为整个文本
				MyInvalidateRect(hTextInfo,
					0, PAINTSIZE(hTextInfo).x,
					STARTPOS(hTextInfo).y - PAINTPOS(hTextInfo).y,
					ENDPOS(hTextInfo).y - PAINTPOS(hTextInfo).y + CHARSIZE(hTextInfo).y
				); // 设定全选后的高亮区域为无效区域

				AdjustPaintPos(hTextInfo); // 滑动显示区 使光标落在显示区内部
			}
			break;
			default:
				break;
			}
		}
		break;
		case IDM_DELETE:
			SendMessage(hWnd, WM_KEYDOWN, VK_DELETE, NULL);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		UpdateWindow(hWnd);
	}
	break;
	case WM_CONTEXTMENU:
	{
		POINT pCursor{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		ScreenToClient(hWnd, &pCursor);

		if (INRANGEX(pCursor.x, 0, PAINTSIZE(hTextInfo).x)
			&& INRANGEY(pCursor.y, 0, PAINTSIZE(hTextInfo).y))
		{
			HMENU hRoot = LoadMenu((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU));
			if (hRoot)
			{
				HMENU hPop = GetSubMenu(hRoot, 0);
				ClientToScreen(hWnd, &pCursor);
				TrackPopupMenu(hPop,
					TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,
					pCursor.x,
					pCursor.y,
					0,
					hWnd,
					NULL
				);
				DestroyMenu(hRoot);
			}
		}
	}
	break;
	case WM_CLOSE:
	{
		switch (UserMessageProc(HTEXT(hTextInfo), NULL, NULL, UM_ISSAVED, NULL, NULL, NULL))
		{	// 查询是否已保存文件
		case UR_NOTSAVED:
		{	// 用户未保存当前文件
			switch (MessageBox(hWnd, TEXT("是否保存更改？"), TEXT("提示"), MB_YESNOCANCEL))
			{
			case IDYES:
				SendMessage(hWnd, WM_COMMAND, IDM_SAVE, NULL);	// 提示用户保存
			case IDNO:
				ReleaseTextInfo(hWnd, hTextInfo);
				DestroyWindow(hWnd);
				break;
			case IDCANCEL:
				break;
			default:
				break;
			}
		}
		break;
		case UR_SAVED:
			ReleaseTextInfo(hWnd, hTextInfo);
			DestroyWindow(hWnd);
			break;
		default:
			break;
		}
	}
	break;
	case WM_SIZE:
	{
		SendMessage(hTextInfo->m_hGDI->m_hStatus, message, wParam, lParam);

		RECT rect;
		GetClientRect(hWnd, &rect);
		SelectClientSize(hTextInfo, &rect);

		InvalidateRect(hWnd, NULL, FALSE); UpdateWindow(hWnd);
	}
	break;
	case WM_SETFOCUS:
	{	// 获得焦点
		CreateCaret(hWnd, NULL,
			CARETSIZE(hTextInfo).x, CARETSIZE(hTextInfo).y
		); // 新建光标
		SelectCaretPos(hTextInfo, CARETPOS(hTextInfo), CARETCOORD(hTextInfo));	// 设置光标位置
		ShowCaret(hWnd);

		UpdateWindow(hWnd);
	}
	break;
	case WM_KILLFOCUS:
	{	// 释放焦点
		HideCaret(hWnd);
		DestroyCaret();
	}
	break;
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_HOME:
		case VK_END:
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
			MoveCaret(hTextInfo, (UINT)wParam);
			break;
		case VK_DELETE:
		{
			KERNELINFO kernelinfo;
			switch (UserMessageProc(HTEXT(hTextInfo),
				STARTPOS(hTextInfo).x, STARTPOS(hTextInfo).y,
				UM_DELETE,
				(FPARAM)ENDPOS(hTextInfo).x,
				(SPARAM)ENDPOS(hTextInfo).y, &kernelinfo))
			{
			case UR_SUCCESS:
			{
				MyInvalidateRect(hTextInfo,
					0, PAINTSIZE(hTextInfo).x,
					STARTPOS(hTextInfo).y - PAINTPOS(hTextInfo).y,
					ENDPOS(hTextInfo).y - PAINTPOS(hTextInfo).y + CHARSIZE(hTextInfo).y
				); // 设置删除之前的高亮部分为无效区域

				if (kernelinfo.m_bLineBreak) // 换行符被删除
					MyInvalidateRect(hTextInfo,
						0, PAINTSIZE(hTextInfo).x,
						CARETPOS(hTextInfo).y - PAINTPOS(hTextInfo).y - CHARSIZE(hTextInfo).y,
						PAINTSIZE(hTextInfo).y
					); // 设置重光标位置开始往下都为无效区域

				SelectTextSize(hTextInfo, kernelinfo.m_pTextPixelSize); // 设置删除高亮部分之后的文本像素大小

				AdjustPaintPos(hTextInfo); // 滑动显示区 使光标落在显示区内部
				SelectHighlight(hTextInfo, CARETPOS(hTextInfo), CARETPOS(hTextInfo)); // 设置无高亮部分
			}
			break;
			default:
				break;
			}
		}
		break;
		default:
			break;
		}
		UpdateWindow(hWnd);
	}
	break;
	case WM_CHAR:
	{
		for (int i = 0; i < (int)LOWORD(lParam); ++i)
		{
			switch (wParam)
			{
			case '\b':
			{
				AdjustCaretPosBeforeBackspace(hTextInfo); // 校准光标位置

				if (STARTPOS(hTextInfo) == POINT{ 0, 0 }
					&& ENDPOS(hTextInfo) == POINT{ 0, 0 })
					return DefWindowProc(hWnd, message, wParam, lParam);
				else // 执行删除
					SendMessage(hWnd, WM_KEYDOWN, VK_DELETE, NULL);
			}
			break;
			case '\n':
			{
			}
			break;
			case '\r':
			{
				KERNELINFO kernelinfo;
				switch (UserMessageProc(HTEXT(hTextInfo),
					CARETPOS(hTextInfo).x, CARETPOS(hTextInfo).y,
					UM_RETURN, NULL, NULL, &kernelinfo))
				{
				case UR_SUCCESS:
				{
					SelectTextSize(hTextInfo, kernelinfo.m_pTextPixelSize); // 设置文本的像素大小
					SelectCaretPos(hTextInfo, kernelinfo.m_pCaretPixelPos, kernelinfo.m_cCaretCoord); // 设置光标的像素位置

					MyInvalidateRect(hTextInfo,
						0, PAINTSIZE(hTextInfo).x,
						STARTPOS(hTextInfo).y - PAINTPOS(hTextInfo).y,
						ENDPOS(hTextInfo).y - PAINTPOS(hTextInfo).y + CHARSIZE(hTextInfo).y
					); // 设置插入换行符之前的高亮部分为无效区域
					SelectHighlight(hTextInfo, CARETPOS(hTextInfo), CARETPOS(hTextInfo)); // 设置无高亮部分
					MyInvalidateRect(hTextInfo,
						0, PAINTSIZE(hTextInfo).x,
						CARETPOS(hTextInfo).y - PAINTPOS(hTextInfo).y - CHARSIZE(hTextInfo).y,
						PAINTSIZE(hTextInfo).y
					); // 设置插入换行符之后上一行至底部均为无效区域

					AdjustPaintPos(hTextInfo); // 滑动显示区 使光标落在显示区内
				}
				break;
				default:
					break;
				}
			}
			break;
			case 0x1B:
			{
				SendMessage(hWnd, WM_CLOSE, NULL, NULL);
			}
			break;
			default:
			{
				if (GetAsyncKeyState(VK_CONTROL) < 0)
				{	// 非字符处理（控制命令/快捷键）
					if (wParam == 0x01) // Ctrl + A 全选
						SendMessage(hWnd, WM_COMMAND, IDM_ALL, NULL);
					else if (wParam == 0x03) // Ctrl + C 拷贝
						SendMessage(hWnd, WM_COMMAND, IDM_COPY, NULL);
					else if (wParam == 0x16) // Ctrl + V 粘贴
						SendMessage(hWnd, WM_COMMAND, IDM_PASTE, NULL);
					else if (wParam == 0x18) // Ctrl + X 剪切
						SendMessage(hWnd, WM_COMMAND, IDM_CUT, NULL);
					else if (wParam == 0x1A) // Ctrl + Z 撤销
						SendMessage(hWnd, WM_COMMAND, IDM_CANCEL, NULL);
					else if (wParam == 0x06) // Ctrl + F 查找
						SendMessage(hWnd, WM_COMMAND, IDM_FIND, NULL);
					else if (wParam == 0x13) // Ctrl + S 保存
						SendMessage(hWnd, WM_COMMAND, IDM_SAVE, NULL);
					else if (wParam == 0x12)
						SendMessage(hWnd, WM_COMMAND, IDM_REPLACE, NULL);
				}
				else
				{
					// 字符处理
					HideCaret(hWnd);

					KERNELINFO kernelinfo;
					switch (UserMessageProc(HTEXT(hTextInfo),
						CARETPOS(hTextInfo).x, CARETPOS(hTextInfo).y,
						UM_CHAR,
						NULL, (SPARAM)wParam, &kernelinfo))
					{
					case UR_SUCCESS:
					{
						SelectTextSize(hTextInfo, kernelinfo.m_pTextPixelSize); // 设置文本的像素大小
						SelectCaretPos(hTextInfo, kernelinfo.m_pCaretPixelPos, kernelinfo.m_cCaretCoord); // 设置光标的像素位置

						MyInvalidateRect(hTextInfo,
							0, PAINTSIZE(hTextInfo).x,
							STARTPOS(hTextInfo).y - PAINTPOS(hTextInfo).y,
							ENDPOS(hTextInfo).y - PAINTPOS(hTextInfo).y + CHARSIZE(hTextInfo).y
						); // 设置插入换行符之前的高亮部分为无效区域

						if (kernelinfo.m_bLineBreak) // 换行符被替换
							MyInvalidateRect(hTextInfo,
								0, PAINTSIZE(hTextInfo).x,
								CARETPOS(hTextInfo).y - PAINTPOS(hTextInfo).y - CHARSIZE(hTextInfo).y,
								PAINTSIZE(hTextInfo).y
							); // 设置插入换行符之后上一行至底部均为无效区域

						SelectHighlight(hTextInfo, CARETPOS(hTextInfo), CARETPOS(hTextInfo)); // 设置无高亮部分

						AdjustPaintPos(hTextInfo); // 滑动显示区 使光标落在显示区内
					}
					break;
					default:
						break;
					}

					ShowCaret(hWnd);
				}
			}
			break;
			}
		}
		UpdateWindow(hWnd);
	}
	break;
	case WM_VSCROLL:
	{
		// 获取滚动条信息
		SCROLLINFO	svInfo;
		svInfo.cbSize = sizeof(SCROLLINFO);
		svInfo.fMask  = SIF_ALL;
		GetScrollInfo(hWnd, SB_VERT, &svInfo);

		int iVertPos = svInfo.nPos;	// 记录初始位置
		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			svInfo.nPos = max(svInfo.nPos - 1, svInfo.nMin);
			break;
		case SB_LINEDOWN:
			svInfo.nPos = min(svInfo.nPos + 1, svInfo.nMax - (int)svInfo.nPage + 1);
			break;
		case SB_PAGEUP:
			svInfo.nPos = max(svInfo.nPos - (int)svInfo.nPage, svInfo.nMin);
			break;
		case SB_PAGEDOWN:
			svInfo.nPos = min(svInfo.nPos + (int)svInfo.nPage, svInfo.nMax - (int)svInfo.nPage + 1);
			break;
		case SB_TOP:
			svInfo.nPos = svInfo.nMin;
			break;
		case SB_BOTTOM:
			svInfo.nPos = svInfo.nMax - svInfo.nPage + 1;
			break;
		case SB_THUMBTRACK:
			if (svInfo.nTrackPos < svInfo.nMin)
				svInfo.nPos = svInfo.nMin;
			else if (svInfo.nTrackPos > svInfo.nMax - (int)svInfo.nPage + 1)
				svInfo.nPos = svInfo.nMax - svInfo.nPage + 1;
			else
				svInfo.nPos = svInfo.nTrackPos;
			break;
		default:
			break;
		}

		// 更新滚动条
		svInfo.cbSize = sizeof(SCROLLINFO);
		svInfo.fMask  = SIF_POS | SIF_DISABLENOSCROLL;
		SetScrollInfo(hWnd, SB_VERT, &svInfo, TRUE);

		SelectPaintPos(hTextInfo, 
			POINT{ PAINTPOS(hTextInfo).x, svInfo.nPos * CHARSIZE(hTextInfo).y }); // 设置绘图区域的像素位置
		SelectCaretPos(hTextInfo, CARETPOS(hTextInfo), CARETCOORD(hTextInfo));	// 设置光标的像素位置

		MyScrollWindow(hTextInfo, 0, (iVertPos - svInfo.nPos) * CHARSIZE(hTextInfo).y); // 滑动绘图区域
		UpdateWindow(hWnd);
	}
	break;
	case WM_HSCROLL:
	{
		// 获取滚动条信息
		SCROLLINFO shInfo;
		shInfo.cbSize = sizeof(SCROLLINFO);
		shInfo.fMask  = SIF_ALL;
		GetScrollInfo(hWnd, SB_HORZ, &shInfo);

		int iHorzPos = shInfo.nPos;	// 记录初始位置
		switch (LOWORD(wParam))
		{
		case SB_LINELEFT:
			shInfo.nPos = max(shInfo.nPos - 1, shInfo.nMin);
			break;
		case SB_LINERIGHT:
			shInfo.nPos = min(shInfo.nPos + 1, shInfo.nMax - (int)shInfo.nPage + 1);
			break;
		case SB_PAGELEFT:
			shInfo.nPos = max(shInfo.nPos - (int)shInfo.nPage, shInfo.nMin);
			break;
		case SB_PAGERIGHT:
			shInfo.nPos = min(shInfo.nPos + (int)shInfo.nPage, shInfo.nMax - (int)shInfo.nPage + 1);
			break;
		case SB_LEFT:
			shInfo.nPos = shInfo.nMin;
			break;
		case SB_RIGHT:
			shInfo.nPos = shInfo.nMax - shInfo.nPage + 1;
			break;
		case SB_THUMBTRACK:
			if (shInfo.nTrackPos < shInfo.nMin)
				shInfo.nPos = shInfo.nMin;
			else if (shInfo.nTrackPos > shInfo.nMax - (int)shInfo.nPage + 1)
				shInfo.nPos = shInfo.nMax - shInfo.nPage + 1;
			else
				shInfo.nPos = shInfo.nTrackPos;
			break;
		default:
			break;
		}
		// 更新滚动条
		shInfo.cbSize = sizeof(SCROLLINFO);
		shInfo.fMask  = SIF_POS;
		SetScrollInfo(hWnd, SB_HORZ, &shInfo, TRUE);

		SelectPaintPos(hTextInfo,
			POINT{ shInfo.nPos * CHARSIZE(hTextInfo).x, PAINTPOS(hTextInfo).y }); // 设置绘图区域的像素位置
		SelectCaretPos(hTextInfo, CARETPOS(hTextInfo), CARETCOORD(hTextInfo));	// 设置光标的像素位置

		MyScrollWindow(hTextInfo, (iHorzPos - shInfo.nPos) * CHARSIZE(hTextInfo).x, 0);	// 滑动窗口
		UpdateWindow(hWnd);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		POINT pCursorPixelPos{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) }; // 解析鼠标位置

		if (!INRANGEX(pCursorPixelPos.x, 0, PAGESIZE(hTextInfo).x * CHARSIZE(hTextInfo).x) ||
			!INRANGEY(pCursorPixelPos.y, 0, PAGESIZE(hTextInfo).y * CHARSIZE(hTextInfo).y)) // 未击中显示区
			return DefWindowProc(hWnd, message, wParam, lParam);
		pCursorPixelPos.x += PAINTPOS(hTextInfo).x;
		pCursorPixelPos.y += PAINTPOS(hTextInfo).y;

		MyInvalidateRect(hTextInfo,
			0, PAINTSIZE(hTextInfo).x,
			STARTPOS(hTextInfo).y - PAINTPOS(hTextInfo).y,
			ENDPOS(hTextInfo).y - PAINTPOS(hTextInfo).y + CHARSIZE(hTextInfo).y
		); // 设置左键按下之前的高亮部分为无效区域

		KERNELINFO kernelinfo;
		switch (UserMessageProc(HTEXT(hTextInfo),
			pCursorPixelPos.x, pCursorPixelPos.y, UM_CURSOR, NULL, NULL, &kernelinfo))
		{
		case UR_SUCCESS:
			SelectCaretPos(hTextInfo, kernelinfo.m_pCaretPixelPos, kernelinfo.m_cCaretCoord); // 设置光标的像素位置
			SelectHighlight(hTextInfo, CARETPOS(hTextInfo), CARETPOS(hTextInfo)); // 设置无高亮部分
			break;

		default:
			break;
		}
		UpdateWindow(hWnd);
	}
	break;
	case WM_LBUTTONUP:
	{ // 鼠标左键弹起时 调整高亮的起点和终点
		if (STARTPOS(hTextInfo).y > ENDPOS(hTextInfo).y)
			std::swap(STARTPOS(hTextInfo), ENDPOS(hTextInfo));
		else if (STARTPOS(hTextInfo).y == ENDPOS(hTextInfo).y
			&& STARTPOS(hTextInfo).x > ENDPOS(hTextInfo).x)
			std::swap(STARTPOS(hTextInfo), ENDPOS(hTextInfo));
	}
	break;
	case WM_NCLBUTTONDBLCLK:
	{
		if (wParam != HTCAPTION)
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	break;
	case WM_NCLBUTTONDOWN:
	{
		if (wParam != HTCAPTION)
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	break;
	case WM_MOUSEMOVE:
	{
		switch (wParam)
		{
		case MK_LBUTTON:
		{	// 按住左键
			if (MTIMER(hTextInfo) != nullptr && IsTimerStopped(MTIMER(hTextInfo)) == FALSE)
				return 0;

			if (MTIMER(hTextInfo) != nullptr)
				KillTimer(MTIMER(hTextInfo)), MTIMER(hTextInfo) = nullptr;
			else
				MTIMER(hTextInfo) = CreateTimer(100);

			POINT pCursorPixelPos{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) }; // 解析鼠标位置

			if (pCursorPixelPos.y > PAINTSIZE(hTextInfo).y) // 下移
				SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
			else if (pCursorPixelPos.x > PAINTSIZE(hTextInfo).x) // 右移
				SendMessage(hWnd, WM_HSCROLL, SB_LINERIGHT, NULL);
			else if (pCursorPixelPos.y < CHARSIZE(hTextInfo).y)	// 上移
				SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
			else if (pCursorPixelPos.x < CHARSIZE(hTextInfo).x) // 左移
				SendMessage(hWnd, WM_HSCROLL, SB_LINELEFT, NULL);
			pCursorPixelPos.x += PAINTPOS(hTextInfo).x;
			pCursorPixelPos.y += PAINTPOS(hTextInfo).y;

			// 鼠标移动之前的高亮部分设置为无效区域
			if (STARTPOS(hTextInfo).y > ENDPOS(hTextInfo).y)
				MyInvalidateRect(hTextInfo,
					0, PAINTSIZE(hTextInfo).x,
					ENDPOS(hTextInfo).y - PAINTPOS(hTextInfo).y,
					STARTPOS(hTextInfo).y - PAINTPOS(hTextInfo).y + CHARSIZE(hTextInfo).y
				);
			else
				MyInvalidateRect(hTextInfo,
					0, PAINTSIZE(hTextInfo).x,
					STARTPOS(hTextInfo).y - PAINTPOS(hTextInfo).y,
					ENDPOS(hTextInfo).y - PAINTPOS(hTextInfo).y + CHARSIZE(hTextInfo).y
				);

#ifdef DEBUG
			wprintf_s(TEXT("Before mouse move : ")); PRINTSTARTPOS(hTextInfo); PRINTENDPOS(hTextInfo); PRINTENDL;
#endif // DEBUG

#ifdef DEBUG
			wprintf_s(TEXT("Cursor' position : (%d, %d)\n"), pCursorPixelPos.x, pCursorPixelPos.y);
#endif // DEBUG

			KERNELINFO kernelinfo;
			switch (UserMessageProc(HTEXT(hTextInfo),
				pCursorPixelPos.x, pCursorPixelPos.y,
				UM_CHOOSE,
				(FPARAM)STARTPOS(hTextInfo).x, (SPARAM)STARTPOS(hTextInfo).y, &kernelinfo))
			{
			case UR_SUCCESS:
			{
				SelectCaretPos(hTextInfo, kernelinfo.m_pCaretPixelPos, kernelinfo.m_cCaretCoord); // 设置光标的像素位置
				SelectHighlight(hTextInfo, STARTPOS(hTextInfo), CARETPOS(hTextInfo)); // 设置高亮部分

				if (STARTPOS(hTextInfo).y > ENDPOS(hTextInfo).y)
					MyInvalidateRect(hTextInfo,
						0, PAINTSIZE(hTextInfo).x,
						ENDPOS(hTextInfo).y - PAINTPOS(hTextInfo).y,
						STARTPOS(hTextInfo).y - PAINTPOS(hTextInfo).y + CHARSIZE(hTextInfo).y
					);
				else
					MyInvalidateRect(hTextInfo,
						0, PAINTSIZE(hTextInfo).x,
						STARTPOS(hTextInfo).y - PAINTPOS(hTextInfo).y,
						ENDPOS(hTextInfo).y - PAINTPOS(hTextInfo).y + CHARSIZE(hTextInfo).y
					);
			}
			break;
			default:
				break;
			}

#ifdef DEBUG
			wprintf_s(TEXT("After mouse move : ")); PRINTSTARTPOS(hTextInfo); PRINTENDPOS(hTextInfo); PRINTENDL;
#endif // DEBUG
			UpdateWindow(hWnd);
		}
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_MOUSEWHEEL:
	{
		switch (LOWORD(wParam))
		{
		case MK_CONTROL:
		{
			if ((short)HIWORD(wParam) > 0)
				SelectCharSize(hTextInfo, 
					min(CHARSIZE(hTextInfo).x + 1, MAXCHARSIZE(hTextInfo).x),
					min(CHARSIZE(hTextInfo).y + 2, MAXCHARSIZE(hTextInfo).y)
				);
			else
				SelectCharSize(hTextInfo,
					max(CHARSIZE(hTextInfo).x - 1, MINCHARSIZE(hTextInfo).x),
					max(CHARSIZE(hTextInfo).y - 2, MINCHARSIZE(hTextInfo).y)
				);
			UpdateWindow(hWnd);
		}
		break;
		default:
		{
			if ((short)HIWORD(wParam) > 0)
				SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
			else
				SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
		}
		break;
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		RECT rcRepaint;
		rcRepaint.left   = max(ps.rcPaint.left, 0);
		rcRepaint.right  = min(ps.rcPaint.right, PAINTSIZE(hTextInfo).x);
		rcRepaint.top    = max(ps.rcPaint.top, 0);
		rcRepaint.bottom = min(ps.rcPaint.bottom, PAINTSIZE(hTextInfo).y);

		PaintWindow(&rcRepaint, hTextInfo);

		BitBlt(hdc,
			rcRepaint.left, rcRepaint.top, rcRepaint.right - rcRepaint.left, rcRepaint.bottom - rcRepaint.top,
			MEMDC(hTextInfo), rcRepaint.left, rcRepaint.top, SRCCOPY);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
	{
#ifdef DEBUG
		FreeConsole();
#endif // DEBUG

		DestroyCaret();
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// “查找”框的消息处理程序。
INT_PTR CALLBACK Find(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	static BOOL s_bFound = false;
	switch (message)
	{
	case WM_INITDIALOG:
	{
		if (MASK(hTextInfo) & FIND)
		{
			SetWindowText(hDlg, TEXT("请输入待查找的字符串"));
			Edit_Enable(GetDlgItem(hDlg, IDC_REPLACEWHAT), FALSE);
			Edit_Enable(GetDlgItem(hDlg, IDC_REPLACECURRENT), FALSE);
		}
		else if (MASK(hTextInfo) & REPLACE)
			SetWindowText(hDlg, TEXT("请输入待替换的字符串"));

		Edit_LimitText(GetDlgItem(hDlg, IDC_FINDWHAT), MAX_LOADSTRING);
		Edit_LimitText(GetDlgItem(hDlg, IDC_REPLACEWHAT), MAX_LOADSTRING);

		TCHAR szTypeItem[MAX_LOADSTRING];
		wsprintf(szTypeItem, TEXT("从光标处向后查找"));
		SendDlgItemMessage(hDlg, IDC_TYPEHINT, CB_ADDSTRING, NULL, (LPARAM)szTypeItem);
		wsprintf(szTypeItem, TEXT("从光标处向前查找"));
		SendDlgItemMessage(hDlg, IDC_TYPEHINT, CB_ADDSTRING, NULL, (LPARAM)szTypeItem);
		wsprintf(szTypeItem, TEXT("当前文档"));
		SendDlgItemMessage(hDlg, IDC_TYPEHINT, CB_ADDSTRING, NULL, (LPARAM)szTypeItem);
		SendDlgItemMessage(hDlg, IDC_TYPEHINT, CB_SETCURSEL, 0, 0);
	}
	return (INT_PTR)TRUE;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_FINDNEXT:
		{
			GetDlgItemText(hDlg, IDC_FINDWHAT, FINDSTRING(hTextInfo), MAX_LOADSTRING);

			if (IsDlgButtonChecked(hDlg, IDC_LHCASE) == BST_CHECKED)
				MASK(hTextInfo) |= LHCASE;

			switch (SendDlgItemMessage(hDlg, IDC_TYPEHINT, CB_GETCURSEL, NULL, NULL))
			{
			case 0:	// 光标后查找
				MASK(hTextInfo) |= AFTER_CARET;
				break;
			case 1:	// 光标前查找
				MASK(hTextInfo) |= BEFORE_CARET;
				break;
			case 2:	// 当前文档查找
				MASK(hTextInfo) |= WHOLE_TEXT;
				break;
			default:
				break;
			}

			KERNELINFO kernelinfo;
			switch (UserMessageProc(HTEXT(hTextInfo),
				CARETPOS(hTextInfo).x, CARETPOS(hTextInfo).y,
				UM_FIND, (FPARAM)FINDSTRING(hTextInfo), (SPARAM)MASK(hTextInfo), &kernelinfo))
			{
			case UR_SUCCESS:
			{
				SelectCaretPos(hTextInfo, kernelinfo.m_pCaretPixelPos, kernelinfo.m_cCaretCoord);

				MyInvalidateRect(hTextInfo,
					0, PAINTSIZE(hTextInfo).x,
					STARTPOS(hTextInfo).y - PAINTPOS(hTextInfo).y,
					ENDPOS(hTextInfo).y - PAINTPOS(hTextInfo).y + CHARSIZE(hTextInfo).y
				); // 设置查找之前的高亮部分为无效区域
				SelectHighlight(hTextInfo, kernelinfo.m_pStartPixelPos, kernelinfo.m_pEndPixelPos);
				MyInvalidateRect(hTextInfo,
					0, PAINTSIZE(hTextInfo).x,
					STARTPOS(hTextInfo).y - PAINTPOS(hTextInfo).y,
					ENDPOS(hTextInfo).y - PAINTPOS(hTextInfo).y + CHARSIZE(hTextInfo).y
				); // 设置查找之后的高亮部分为无效区域

				AdjustPaintPos(hTextInfo); // 滑动绘图区域
				s_bFound = true;
			}
			break;
			default:
				MessageBox(hTextInfo->m_hWnd, TEXT("无当前查找项"), NULL, MB_OK);
				break;
			}
		}
		return (INT_PTR)TRUE;

		case IDC_REPLACECURRENT:
		{
			if (s_bFound && MessageBox(hTextInfo->m_hWnd,
				TEXT("确定要替换该查找项？"), TEXT("提示"), MB_OKCANCEL) == IDOK)
			{
				KERNELINFO kernelinfo;
				switch (UserMessageProc(HTEXT(hTextInfo),
					NULL, NULL,
					UM_REPLACE,
					NULL, (SPARAM)REPLACESTRING(hTextInfo),
					&kernelinfo))
				{
				case UR_SUCCESS:
				{
					SelectTextSize(hTextInfo, kernelinfo.m_pTextPixelSize);	// 设置文本范围

					MyInvalidateRect(hTextInfo,
						0, PAINTSIZE(hTextInfo).x,
						STARTPOS(hTextInfo).y - PAINTPOS(hTextInfo).y,
						ENDPOS(hTextInfo).y - PAINTPOS(hTextInfo).y + CHARSIZE(hTextInfo).y
					); // 设置替换之前的高亮部分为无效区域
					SelectHighlight(hTextInfo, kernelinfo.m_pStartPixelPos, kernelinfo.m_pEndPixelPos);
					MyInvalidateRect(hTextInfo,
						0, PAINTSIZE(hTextInfo).x,
						STARTPOS(hTextInfo).y - PAINTPOS(hTextInfo).y,
						ENDPOS(hTextInfo).y - PAINTPOS(hTextInfo).y + CHARSIZE(hTextInfo).y
					); // 设置替换之后的高亮部分为无效区域
					s_bFound = false;
				}
				break;
				default:
					break;
				}
			}
		}
		return (INT_PTR)TRUE;

		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
		return (INT_PTR)TRUE;
		}
	}
	break;
	}
	return (INT_PTR)FALSE;
}