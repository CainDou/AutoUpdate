// AutoUpdateWin.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "AutoUpdate.h"
#include "AutoUpdateHandle.h"

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
CAutoUpdateHandle *pUpdateHandle;
HWND hWnd;										//窗口句柄
// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL				CenterWindow(HWND hWnd);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	nCmdShow = SW_HIDE;
	// TODO: 在此放置代码。
	
	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_AUTOUPDATEWIN, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AUTOUPDATEWIN));

	MSG msg;

	pUpdateHandle = new CAutoUpdateHandle;
	pUpdateHandle->Run(hWnd);
	// 主消息循环: 
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	delete pUpdateHandle;
	pUpdateHandle = nullptr;
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

	wcex.style = CS_HREDRAW | CS_VREDRAW | DS_CENTER | CS_NOCLOSE | CS_NOMOVECARET;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AUTOUPDATEWIN));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_AUTOUPDATEWIN);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

	hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED,
		0, 0, 400, 350, nullptr, nullptr, hInstance, nullptr);

	CenterWindow(hWnd);

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
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		HBITMAP	hbm;
		BITMAP	bminfo;
		hbm = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1));
		GetObject(hbm, sizeof(bminfo), &bminfo);

		PAINTSTRUCT	ps;
		HDC dc = BeginPaint(hWnd, &ps);

		HDC	memdc = CreateCompatibleDC(dc);

		SelectObject(memdc, hbm);
		BitBlt(dc, 0, 0, bminfo.bmWidth, bminfo.bmHeight, memdc, 0, 0, SRCCOPY);

		DeleteDC(memdc);

		EndPaint(hWnd, &ps);

	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_WINDOW_MSG:
		pUpdateHandle->OnMsg(wParam, lParam);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

BOOL CenterWindow(HWND hWnd)
{

	// determine owner window to center against
	HWND hWndCenter = ::GetWindow(hWnd, GW_OWNER);

	if (hWndCenter == NULL)
		hWndCenter = ::GetActiveWindow();

	// get coordinates of the window relative to its parent
	RECT rcDlg;
	::GetWindowRect(hWnd, &rcDlg);
	RECT rcArea;
	RECT rcCenter;

	// don't center against invisible or minimized windows
	if (hWndCenter != NULL)
	{
		DWORD dwStyleCenter = ::GetWindowLongPtr(hWndCenter, GWL_STYLE);
		if (!(dwStyleCenter & WS_VISIBLE) || (dwStyleCenter & WS_MINIMIZE))
			hWndCenter = NULL;
	}

	// center within screen coordinates
	HMONITOR hMonitor = NULL;
	if (hWndCenter != NULL)
	{
		hMonitor = ::MonitorFromWindow(hWndCenter, MONITOR_DEFAULTTONEAREST);
	}
	else
	{
		hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	}

	MONITORINFO minfo;
	minfo.cbSize = sizeof(MONITORINFO);
	::GetMonitorInfo(hMonitor, &minfo);

	rcArea = minfo.rcWork;
	if (hWndCenter == NULL)
		rcCenter = rcArea;
	else
		::GetWindowRect(hWndCenter, &rcCenter);

	int DlgWidth = rcDlg.right - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;

	// find dialog's upper left based on rcCenter
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

	// if the dialog is outside the screen, move it inside
	if (xLeft + DlgWidth > rcArea.right)
		xLeft = rcArea.right - DlgWidth;
	if (xLeft < rcArea.left)
		xLeft = rcArea.left;

	if (yTop + DlgHeight > rcArea.bottom)
		yTop = rcArea.bottom - DlgHeight;
	if (yTop < rcArea.top)
		yTop = rcArea.top;

	// map screen coordinates to child coordinates
	return ::SetWindowPos(hWnd, NULL, xLeft, yTop, -1, -1,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

