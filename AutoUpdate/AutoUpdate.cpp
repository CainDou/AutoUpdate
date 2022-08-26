// AutoUpdateWin.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "AutoUpdate.h"
#include "AutoUpdateHandle.h"

#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������
CAutoUpdateHandle *pUpdateHandle;
HWND hWnd;										//���ھ��
// �˴���ģ���а����ĺ�����ǰ������: 
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
	// TODO: �ڴ˷��ô��롣
	
	// ��ʼ��ȫ���ַ���
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_AUTOUPDATEWIN, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AUTOUPDATEWIN));

	MSG msg;

	pUpdateHandle = new CAutoUpdateHandle;
	pUpdateHandle->Run(hWnd);
	// ����Ϣѭ��: 
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
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
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
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

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
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// �����˵�ѡ��: 
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

