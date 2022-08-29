#include "stdafx.h"
#include "AutoUpdateHandle.h"
#include "IniFile.h"
#include <fstream>
#include <direct.h>
#include <string>

using namespace std;

CAutoUpdateHandle::CAutoUpdateHandle()
{
}


CAutoUpdateHandle::~CAutoUpdateHandle()
{
}


void CAutoUpdateHandle::Run(HWND hWnd)
{
	InitNetConfig();
	InitFileVer();
	InitNetHandleMap();
	m_hWnd = hWnd;
	m_NetClient.SetWndHandle(m_hWnd);
	m_NetClient.RegisterHandle(NetHandle);
	m_NetClient.Start(m_uNetThreadID, this);
	m_NetClient.OnConnect(m_strIPAddr.c_str(), m_nIPPort);
	_mkdir(".\\tmpFile");
	SendInfo si = { 0 };
	si.MsgType = SendMsg_ClientFileVer;
	strcpy_s(si.Message, m_strClientName.c_str());
	::send(m_NetClient.GetSocket(), (char*)&si, sizeof(si), 0);
}

void CAutoUpdateHandle::OnClose()
{
	_rmdir(".\\tmpFile");
	::PostMessage(m_hWnd, WM_QUIT, NULL, NULL);
}

void CAutoUpdateHandle::InitNetConfig()
{
	CIniFile ini(".//config//NetConfig.ini");
	m_strIPAddr = ini.GetStringA("IP", "Addr", "");
	m_nIPPort = ini.GetIntA("IP", "Port", 0);

}

void CAutoUpdateHandle::InitFileVer()
{
	CIniFile ini(".//config//FileVer.ini");
	m_strClientName = ini.GetStringA("Client", "Name", "");
	m_StartFile = ini.GetStringA("Client", "StartFile", "");
	int nFileCount = ini.GetIntA("File", "Count", 0);
	if (nFileCount > 0)
	{
		string strFileName;
		string strMD5;
		for (int i = 0; i < nFileCount; ++i)
		{
			strFileName = ini.GetStringA("File", to_string(i).c_str(), "");
			if (strFileName != "")
			{
				strMD5 = ini.GetStringA("MD5", strFileName.c_str() , "");
				if (strMD5 == "")
				{
					string strFile = ".\\" + strFileName;
					ifstream ifile(strFile, std::ios::binary);
					if (ifile.is_open())
					{
						m_MD5.reset();
						m_MD5.update(ifile);
						strMD5 = m_MD5.toString().c_str();
						ifile.close();
					}
				}
				FileVer fv = { "" };
				strcpy_s(fv.FileName, strFileName.c_str());
				strcpy_s(fv.MD5, strMD5.c_str());
				m_SrcFileMap[fv.FileName] = fv;
			}
		}
	}
}

void CAutoUpdateHandle::InitNetHandleMap()
{
	m_netHandleMap[RecvMsg_ClientFileVer]
		= &CAutoUpdateHandle::OnMsgFileVer;
	m_netHandleMap[RecvMsg_ClientFile]
		= &CAutoUpdateHandle::OnMsgFile;
}

BOOL CAutoUpdateHandle::ReceiveData(SOCKET socket, int size, char end, char * buffer, int offset)
{
	char*p = buffer + offset;
	int sizeLeft = size;
	while (sizeLeft > 0)
	{
		int ret = recv(socket, p, sizeLeft, 0);
		if (SOCKET_ERROR == ret)
		{
			delete[] buffer;
			buffer = nullptr;
			p = nullptr;
			return 0;
		}
		sizeLeft -= ret;
		p += ret;
	}
	p = nullptr;
	char cEnd;
	int ret = recv(socket, &cEnd, 1, 0);
	if (cEnd == end)
		return true;
	return false;
}

BOOL CAutoUpdateHandle::RecvInfoHandle(int & nOffset, ReceiveInfo & recvInfo)
{
	int ret = recv(m_NetClient.GetSocket(),
		(char*)&recvInfo + nOffset,
		sizeof(recvInfo) - nOffset, 0);
	if (ret == 0)
	{
		nOffset = 0;
		m_NetClient.OnConnect(NULL, NULL);
		return false;
	}

	if (SOCKET_ERROR == ret)
	{
		//nOffset = 0;
		if (m_NetClient.GetExitState())
			return false;
		int nError = WSAGetLastError();
		if (nError == WSAECONNRESET)
			m_NetClient.OnConnect(NULL, NULL);
		return false;
	}

	if (ret + nOffset < sizeof(recvInfo))
	{
		nOffset += ret;
		return false;
	}
	nOffset = 0;

	return true;
}

unsigned CAutoUpdateHandle::NetHandle(void * para)
{
	CAutoUpdateHandle * pMd = (CAutoUpdateHandle*)para;
	int nOffset = 0;
	ReceiveInfo recvInfo = { 0 };
	while (true)
	{
		if (pMd->m_NetClient.GetExitState())
			return 0;
		if (pMd->RecvInfoHandle(nOffset, recvInfo))
		{
			auto pFuc = pMd->m_netHandleMap[recvInfo.MsgType];
			if (pFuc == nullptr)
				pFuc = &CAutoUpdateHandle::OnNoDefineMsg;
			(pMd->*pFuc)(pMd->m_NetClient.GetSocket(), recvInfo);
		}

	}
	return 0;
}

LRESULT CAutoUpdateHandle::OnMsg(WPARAM wp, LPARAM lp)
{
	int nMsg = (int)wp;
	switch (nMsg)
	{
	case WW_CheckFile:
		CheckFile();
		break;
	case WW_SaveFile:
	{
		char* strFileName = (char*)lp;
		UpdateFileState(strFileName);
		delete[]strFileName;
	}
	break;
	default:
		break;
	}
	return 0;
}

void CAutoUpdateHandle::OnMsgFileVer(SOCKET netSocket, ReceiveInfo & recvInfo)
{
	char *buffer = new char[recvInfo.SendDataSize];
	if (ReceiveData(netSocket, recvInfo.SendDataSize, '#', buffer))
	{
		int nDataCount = recvInfo.SendDataSize / sizeof(FileVer);
		FileVer * fileVerArr = (FileVer *)buffer;
		for (int i = 0; i < nDataCount; ++i)
			m_GetFileMap[fileVerArr[i].FileName] = fileVerArr[i];
	}
	delete[]buffer;
	buffer = nullptr;
	::PostMessage(m_hWnd, WM_WINDOW_MSG, WW_CheckFile, NULL);
}

void CAutoUpdateHandle::OnMsgFile(SOCKET netSocket, ReceiveInfo & recvInfo)
{
	char *buffer = new char[recvInfo.SendDataSize];
	if (ReceiveData(netSocket, recvInfo.SendDataSize, '#', buffer))
	{
		char* FileName = new char[MAX_PATH];
		memcpy_s(FileName, MAX_PATH, buffer, MAX_PATH);
		m_MD5.reset();
		m_MD5.update(buffer + MAX_PATH, recvInfo.SrcDataSize);
		string MD5 = m_MD5.toString();
		if (MD5 == m_GetFileMap[FileName].MD5)
		{
			char strFile[MAX_PATH];
			sprintf_s(strFile,".\\tmpFile\\%s", FileName);
			ofstream ofile(strFile, std::ios::binary);
			if (ofile.is_open())
			{
				ofile.write(buffer + MAX_PATH, recvInfo.SrcDataSize);
				ofile.close();
			}
		}
		::PostMessage(m_hWnd, WM_WINDOW_MSG, WW_SaveFile, (LPARAM)FileName);
	}
	delete[]buffer;
	buffer = nullptr;

}

void CAutoUpdateHandle::OnNoDefineMsg(SOCKET netSocket, ReceiveInfo & recvInfo)
{
	char *buffer = new char[recvInfo.SendDataSize];
	ReceiveData(netSocket, recvInfo.SendDataSize, '#', buffer);
	delete[]buffer;
	buffer = nullptr;
}

void CAutoUpdateHandle::CheckFile()
{
	for (auto &it : m_GetFileMap)
	{
		if (m_SrcFileMap.count(it.first))
		{
			if (strcmp(m_SrcFileMap[it.first].MD5, it.second.MD5))
				m_DiffSet.insert(it.first);
		}
		else
			m_DiffSet.insert(it.first);
	}
	for (auto &it : m_SrcFileMap)
	{
		if (m_GetFileMap.count(it.first) == 0)
			m_RemoveSet.insert(it.first);
	}
	if (m_DiffSet.empty() && m_RemoveSet.empty())
	{
		StartClient();
		OnClose();
	}
	else if (m_DiffSet.empty())
	{
		for (auto &it : m_RemoveSet)
		{
			string strFile = ".\\%s" + it;
			remove(strFile.c_str());
		}
		StartClient();
		OnClose();
	}
	else
	{
		::ShowWindow(m_hWnd,SW_SHOW);
		GetClientFile(*m_DiffSet.begin());
	}

}

void CAutoUpdateHandle::UpdateFileState(string strFileName)
{
	Sleep(100);
	m_DiffSet.erase(strFileName);
	m_GetSet.insert(strFileName);
	if (!m_DiffSet.empty())
		GetClientFile(*m_DiffSet.begin());
	else
	{
	
		for (auto &it : m_GetSet)
		{
			UpdateLog(it.c_str(), LMT_MoveFile);
			char strSrcFile[MAX_PATH] = { 0 };
			char strDstFile[MAX_PATH] = { 0 };
			sprintf_s(strSrcFile,".\\tmpFile\\%s", it.c_str());
			sprintf_s(strDstFile, ".\\%s", it.c_str());
			BOOL bRes = MoveFileExA(strSrcFile, strDstFile,
				MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
			Sleep(100);
		}
		if (m_DiffSet.empty())
		{
			UpdateLog("", LMT_MoveFile);
			for (auto &it : m_RemoveSet)
			{
				char strFile[MAX_PATH] = {0};
				sprintf_s(strFile, ".\\%s", it.c_str());
				remove(strFile);
			}
		}
		ReWriteConfig();
		StartClient();
		OnClose();
	}
}

void CAutoUpdateHandle::UpdateLog(const char* strMsg, LogMsgType nMsgType)
{
	HDC hdc = ::GetDC(m_hWnd);
	char strLog[1024] = {0};
	switch (nMsgType)
	{
	case LMT_GetFile:
		sprintf_s(strLog,"正在获取:%s",strMsg);
		break;
	case LMT_MoveFile:
		sprintf_s(strLog, "正在应用更新:%s", strMsg);
		break;
	case LMT_ClearFile:
		sprintf_s(strLog, "正在清除文件...");
		break;
	default:
		break;
	}
	LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));
	lf.lfHeight = 20;// 字体大小
	_stprintf_s(lf.lfFaceName, L"%s", L"微软雅黑"); // 字体名称
	HFONT hf = CreateFontIndirect(&lf);
	SelectObject(hdc, hf);
	RECT rc = { 50,220,350,330 };
	HBRUSH hB = CreateSolidBrush(::GetBkColor(hdc));
	FillRect(hdc, &rc, hB);
	DrawTextA(hdc, strLog, strlen(strLog), &rc, DT_LEFT);
	DeleteObject(hf);
	DeleteObject(hB);
	::ReleaseDC(m_hWnd,hdc);
}

void CAutoUpdateHandle::StartClient()
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	char cmdLine[1024] = { 0 };
	sprintf_s(cmdLine, "%s -NoCheck", m_StartFile.c_str());
	CreateProcessA(NULL, cmdLine, NULL, NULL, FALSE, 0,
		NULL, NULL, &si, &pi);
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);

}

void CAutoUpdateHandle::GetClientFile(string FileName)
{
	UpdateLog(FileName.c_str(), LMT_GetFile);
	SendInfo si = { 0 };
	si.MsgType = SendMsg_ClientFile;
	strcpy_s(si.Message, m_strClientName.c_str());
	int nSize = sizeof(si) + MAX_PATH;
	char *msg = new char[nSize];
	ZeroMemory(msg, nSize);
	memcpy_s(msg, nSize, &si, sizeof(si));
	memcpy_s(msg + sizeof(si), nSize,
		FileName.c_str(), FileName.length());
	::send(m_NetClient.GetSocket(), msg, nSize, 0);
	delete[]msg;
}

void CAutoUpdateHandle::ReWriteConfig()
{
	CIniFile ini(".//config//FileVer.ini");
	m_strClientName = ini.GetStringA("Client", "Name", "");
	m_StartFile = ini.GetStringA("Client", "StartFile", "");
	int nFileCount = ini.GetIntA("File", "Count", 0);
	ini.WriteIntA("File", "Count", m_GetFileMap.size());
	int i = 0;
	for (auto &it : m_GetFileMap)
	{
		ini.WriteStringA("File", to_string(i++).c_str(), it.second.FileName);
		ini.WriteStringA("MD5", it.second.FileName, it.second.MD5);
	}
}
