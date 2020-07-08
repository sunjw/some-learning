// H3CDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

#include <pcap.h>
#include <Iphlpapi.h>
#include <string>
#include <vector>
#include <memory>

#include "HWNetworkWin.h"
#include "ProfileManager.h"

using namespace std;

#define WM_INFO			WM_USER+1
#define WM_INTERRUPTED	WM_USER+2
#define WM_TRAY			WM_USER+3

typedef vector<CString> CStrVector;

struct ThreadInfo
{
	HWND hWnd;
	string username;
	string password;
	string device;
	int mac[6];
	HuaweiNetworkWin* pHWObj;
	BOOL connect;
};

typedef ThreadInfo* pThreadInfo;

const string iniFilePath = "h3c.ini";

// CH3CDlg 对话框
class CH3CDlg : public CDialog
{
// 构造
public:
	CH3CDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_H3C_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	NOTIFYICONDATA tnd;

	auto_ptr<ProfileManager> profileManager;
	bool isShown;
	CString m_userName;
	CString m_passWord;
	CComboBox m_device;
	CString m_textMac;
	string m_deviceUsed;
	int m_deviceNum;
	PIP_ADAPTER_INFO pAdapterInfo;
	//int m_mac[MAX_ADAPTER_ADDRESS_LENGTH];
	ThreadInfo m_threadInfo;

public:

	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedCon();
	afx_msg void OnBnClickedHide();
	afx_msg void OnBnClickedExit();
	afx_msg void OnMenuExit();
	afx_msg void OnMenuMain();
	afx_msg void OnMenuCon();
	afx_msg void OnUpdateMenuCon(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedAbout();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonSetting();
	afx_msg LRESULT OnInfo(WPARAM, LPARAM);
	afx_msg LRESULT OnInterrupted(WPARAM, LPARAM);
	afx_msg LRESULT OnTray(WPARAM, LPARAM);

	static DWORD WINAPI ConProc(LPVOID lpParameter);

private:
	BOOL m_isConnect;
	CString m_infoDisplay;

//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedSave();
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);

	string GetMac(PIP_ADAPTER_INFO pAdapter);
	CString GetMac(int i);
	void GetConfig();
	void SaveConfig();
	void SaveDevice(void);

	CStrVector ParseCmdLine();

	void ShowDialog();
	void HideDialog();

	void SetCtrls(BOOL connected);
	void SetNotify(BOOL connected);

protected:
//	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	
};
