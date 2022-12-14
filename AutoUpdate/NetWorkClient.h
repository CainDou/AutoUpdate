#pragma once
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

/**
* PFNNETHANDLE
* @brief     接收函数--函数指针
*
* Describe   函数指针
*/
typedef unsigned int(__stdcall *PFNNETHANDLE)(void*);

class CNetWorkClient
{
public:
	CNetWorkClient();
	~CNetWorkClient();
	BOOL SetWndHandle(HWND hwnd);
	BOOL ConnectInit(LPCSTR pszRemoteAddr, u_short nPort, HWND hWnd);
	BOOL OnConnect(LPCSTR lpIP, UINT uPort);
	BOOL RegisterHandle(PFNNETHANDLE pFunc);
	HANDLE Start(UINT &ThreadID,void* para);
	BOOL   Stop();
	SOCKET GetSocket() const;
	BOOL   GetState() const;
	BOOL   SetState(BOOL bState);
	BOOL   SetClientID(SOCKET s);
	SOCKET GetClientID() const;
	BOOL   GetExitState() const;
protected:
	SOCKET m_socket;
	SOCKET m_ClientID;
	HWND m_hWnd;
	sockaddr_in m_remote;
	BOOL m_bConnected;
	BOOL m_bExit;
	UINT m_uThreadID;
	HANDLE m_hFunc;
	PFNNETHANDLE m_pFnHandle;
};

inline BOOL CNetWorkClient::SetWndHandle(HWND hWnd)
{
	m_hWnd = hWnd;
	return TRUE;
}


inline SOCKET CNetWorkClient::GetSocket() const
{
	return m_socket;
}

inline BOOL CNetWorkClient::GetState() const
{
	return m_bConnected;
}

inline BOOL CNetWorkClient::SetState(BOOL bState)
{
	m_bConnected = FALSE;
	return TRUE;
}

inline BOOL CNetWorkClient::SetClientID(SOCKET s)
{
	m_ClientID = s;
	return TRUE;
}

inline SOCKET CNetWorkClient::GetClientID() const
{
	return m_ClientID;
}

inline BOOL CNetWorkClient::GetExitState() const
{
	return m_bExit;
}

