#pragma once
#include"NetWorkClient.h"
#include "MD5.h"
#include <map>
#include <unordered_map>
#include <set>

#define WM_WINDOW_MSG (WM_USER+156)

using std::map;
using std::unordered_map;
using std::set;

typedef struct _FileVer
{
	char FileName[MAX_PATH];
	char MD5[33];
}FileVer;

typedef struct _ReceiveInfo
{
	int MsgType;
	char Message[10];
	int ExtraInfo1;
	int ExtraInfo2;
	unsigned long SendDataSize;		//snap数据时 stockIndex的数据大小;kline数据时 压缩后数据大小
	unsigned long SrcDataSize;		//snap数据时 futures的数据大小;kline数据时 原始数据大小
}ReceiveInfo;

typedef struct _SendInfo
{
	int MsgType;
	char Message[10];
	int  ExtraInfo1;	//表示开始日期或者分组
	int  ExtraInfo2;	//表示结束日期或者周期
}SendInfo;

enum ReceiveCommonMsgType
{
	RecvMsg_ClientID = 170000,
	RecvMsg_ClientFileVer,
	RecvMsg_ClientFile,
};

enum SendCommonMsgType
{
	SendMsg_ClientFileVer = 1000,
	SendMsg_ClientFile,
};

enum WindowMsg
{
	WW_CheckFile,
	WW_SaveFile,
	WW_UpdateLog,
};
enum LogMsgType
{
	LMT_GetFile,
	LMT_MoveFile,
	LMT_ClearFile,
};
class CAutoUpdateHandle
{
	typedef void(CAutoUpdateHandle::*PNETHANDLEFUNC)(SOCKET, ReceiveInfo&);
public:
	CAutoUpdateHandle();
	~CAutoUpdateHandle();

	void Run(HWND hWnd);
	void OnClose();
	void InitNetConfig();
	void InitFileVer();
	void InitNetHandleMap();
	BOOL ReceiveData(SOCKET socket, int size, char end,
		char * buffer, int offset = 0);
	BOOL RecvInfoHandle(int &nOffset, ReceiveInfo &recvInfo);
	static unsigned __stdcall NetHandle(void* para);
	LRESULT OnMsg(WPARAM wp, LPARAM lp);
protected:
	void OnMsgFileVer(SOCKET netSocket, ReceiveInfo &recvInfo);
	void OnMsgFile(SOCKET netSocket, ReceiveInfo &recvInfo);
	void OnNoDefineMsg(SOCKET netSocket, ReceiveInfo &recvInfo);
	void CheckFile();
	void UpdateFileState(string strFileName);
	void UpdateLog(const char* strMsg, LogMsgType nMsgType);
	void StartClient();
	void GetClientFile(string FileName);
	void ReWriteConfig();
protected:

	string m_strClientName;
	string m_StartFile;
	map<string, FileVer> m_SrcFileMap;
	map<string, FileVer> m_GetFileMap;
	unordered_map<int, PNETHANDLEFUNC>m_netHandleMap;
	set<string> m_DiffSet;
	set<string> m_GetSet;
	set<string> m_RemoveSet;

	CNetWorkClient m_NetClient;
	UINT		m_uNetThreadID;
	MD5 m_MD5;
	string m_strIPAddr;
	int m_nIPPort;
	HWND m_hWnd;
};

