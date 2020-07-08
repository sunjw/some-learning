// H3CDlg.cpp : 实现文件
//

#include "stdafx.h"

#include <fstream>

#include "H3C.h"
#include "H3CDlg.h"
#include "SettingDialog.h"
#include "AboutDlg.h"
#include "CStringConvert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CH3CDlg 对话框

CH3CDlg::CH3CDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CH3CDlg::IDD, pParent)
	, profileManager(ProfileManager::GetProfileManager(_T("h3c.ini")))
	, m_textMac(_T(""))
	, m_infoDisplay(_T(""))
{
	m_userName = _T("");
	m_passWord = _T("");
	m_deviceNum = 0;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_isConnect = FALSE;
	isShown = true;
	m_threadInfo.pHWObj = NULL;
}

void CH3CDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USERNAME, m_userName);
	DDX_Text(pDX, IDC_PASSWORD, m_passWord);
	DDX_Control(pDX, IDC_COMBO1, m_device);
	DDX_Text(pDX, IDC_MAC, m_textMac);
	DDX_CBIndex(pDX, IDC_COMBO1, m_deviceNum);
	DDX_Text(pDX, IDC_EDIT1, m_infoDisplay);
}

BEGIN_MESSAGE_MAP(CH3CDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO1, &CH3CDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(ID_CON, &CH3CDlg::OnBnClickedCon)
	ON_MESSAGE(WM_INFO,OnInfo)
	ON_MESSAGE(WM_INTERRUPTED,OnInterrupted)
	ON_MESSAGE(WM_TRAY,OnTray)
//	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_BN_CLICKED(ID_SAVE, &CH3CDlg::OnBnClickedSave)
	ON_BN_CLICKED(ID_HIDE, &CH3CDlg::OnBnClickedHide)
	ON_BN_CLICKED(ID_EXIT, &CH3CDlg::OnBnClickedExit)
	ON_COMMAND(ID_MENU_EXIT, &CH3CDlg::OnMenuExit)
	ON_COMMAND(ID_MENU_MAIN, &CH3CDlg::OnMenuMain)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_MENU_CON, &CH3CDlg::OnMenuCon)
	ON_UPDATE_COMMAND_UI(ID_MENU_CON, &CH3CDlg::OnUpdateMenuCon)
	ON_BN_CLICKED(ID_ABOUT, &CH3CDlg::OnBnClickedAbout)
	ON_WM_SYSCOMMAND()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SETTING, &CH3CDlg::OnBnClickedButtonSetting)
END_MESSAGE_MAP()


// CH3CDlg 消息处理程序

BOOL CH3CDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	CreateMutex(NULL, true, _T("Huawei"));
	if(GetLastError() == ERROR_ALREADY_EXISTS)   
	{   
		MessageBoxEx(NULL, 
			_T("已经有一个实例在运行！"),
			_T("H3C 802.1x"),
			MB_OK|MB_APPLMODAL|MB_ICONEXCLAMATION,
			0x0804);
		Shell_NotifyIcon(NIM_DELETE,&tnd);
		PostQuitMessage(0);
		return FALSE;   
	}

	// 读取信息
	GetConfig();

	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;

	pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO) );
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

// Make an initial call to GetAdaptersInfo to get
// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen); 
	}

	int devCount = 0;
	if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR) 
	{
		pAdapter = pAdapterInfo;
		while (pAdapter) 
		{
			string strDescri(pAdapter->Description);
			m_device.AddString(strtotstr(strDescri).c_str());

			string strMac = GetMac(pAdapter);
			if(m_deviceUsed == strMac)
				m_deviceNum = devCount;
			
			pAdapter = pAdapter->Next;
			++devCount;
		}
	}
	//else 
	//{
	//	printf("Call to GetAdaptersInfo failed.\n");
	//}

	if(m_deviceNum >= devCount)
		m_deviceNum = 0;
	GetMac(m_deviceNum);

	UpdateData(FALSE);
	
	CStrVector Paras = ParseCmdLine();
	for(CStrVector::iterator ite = Paras.begin(); ite != Paras.end(); ++ite)
		if(!(*ite).Compare(_T("-mini")))
			SetTimer(1, 200, NULL);

	SetCtrls(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

CStrVector CH3CDlg::ParseCmdLine()
{
	// 从命令行获取文件路径
	CStrVector parameters;
	tstring tstrCmdLine(theApp.m_lpCmdLine);
	size_t cmdLen = tstrCmdLine.length();
	if(cmdLen > 0)
	{
		for(size_t i = 0; i < cmdLen; ++i)
		{
			CString para(_T(""));
			if(tstrCmdLine[i] == _T('"'))
			{
				++i;
				for(; i < cmdLen && tstrCmdLine[i] != _T('"'); ++i)
					para.AppendChar(tstrCmdLine[i]);

				parameters.push_back(para);
				++i;
			}
			else
			{
				for(; i < cmdLen && tstrCmdLine[i] != _T(' '); ++i)
					para.AppendChar(tstrCmdLine[i]);

				parameters.push_back(para);
			}
		}
	}
	// 从命令行获取文件路径结束
	return parameters;
}

void CH3CDlg::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)
{
    ASSERT(pPopupMenu != NULL);
    // Check the enabled state of various menu items.

    CCmdUI state;
    state.m_pMenu = pPopupMenu;
    ASSERT(state.m_pOther == NULL);
    ASSERT(state.m_pParentMenu == NULL);

    // Determine if menu is popup in top-level menu and set m_pOther to
    // it if so (m_pParentMenu == NULL indicates that it is secondary popup).
    HMENU hParentMenu;
    if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
        state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
    else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
    {
        CWnd* pParent = this;
           // Child windows don't have menus--need to go to the top!
        if (pParent != NULL &&
           (hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
        {
           int nIndexMax = ::GetMenuItemCount(hParentMenu);
           for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
           {
            if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
            {
                // When popup is found, m_pParentMenu is containing menu.
                state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
                break;
            }
           }
        }
    }

    state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
    for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
      state.m_nIndex++)
    {
        state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
        if (state.m_nID == 0)
           continue; // Menu separator or invalid cmd - ignore it.

        ASSERT(state.m_pOther == NULL);
        ASSERT(state.m_pMenu != NULL);
        if (state.m_nID == (UINT)-1)
        {
           // Possibly a popup menu, route to first item of that popup.
           state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
           if (state.m_pSubMenu == NULL ||
            (state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
            state.m_nID == (UINT)-1)
           {
            continue;       // First item of popup can't be routed to.
           }
           state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
        }
        else
        {
           // Normal menu item.
           // Auto enable/disable if frame window has m_bAutoMenuEnable
           // set and command is _not_ a system command.
           state.m_pSubMenu = NULL;
           state.DoUpdate(this, FALSE);
        }

        // Adjust for menu deletions and additions.
        UINT nCount = pPopupMenu->GetMenuItemCount();
        if (nCount < state.m_nIndexMax)
        {
           state.m_nIndex -= (state.m_nIndexMax - nCount);
           while (state.m_nIndex < nCount &&
            pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
           {
            state.m_nIndex++;
           }
        }
        state.m_nIndexMax = nCount;
    }
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CH3CDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CH3CDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

string CH3CDlg::GetMac(PIP_ADAPTER_INFO pAdapter)
{
	string ret;
	if(pAdapter != NULL)
	{
		char buf[32];
		sprintf_s(buf, sizeof(buf), 
					"%02X:%02X:%02X:%02X:%02X:%02X",
					pAdapter->Address[0],
					pAdapter->Address[1],
					pAdapter->Address[2],
					pAdapter->Address[3],
					pAdapter->Address[4],
					pAdapter->Address[5]);
		ret = string(buf);
	}

	return ret;
}

CString CH3CDlg::GetMac(int i)
{
	PIP_ADAPTER_INFO pAdapter = NULL;
	pAdapter = pAdapterInfo;
	for(int j = 0; j < i && pAdapter; ++j)
	{
		pAdapter = pAdapter->Next;
	}

	for(int k = 0; k < 6; k++)
	{
		m_threadInfo.mac[k] = pAdapter->Address[k];
	}

	m_threadInfo.device = pAdapter->AdapterName;
	m_textMac.Format(_T("%02X:%02X:%02X:%02X:%02X:%02X"),
					pAdapter->Address[0],
					pAdapter->Address[1],
					pAdapter->Address[2],
					pAdapter->Address[3],
					pAdapter->Address[4],
					pAdapter->Address[5]);

	UpdateData(FALSE);

	return m_textMac;
}

void CH3CDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	GetMac(m_deviceNum);

	SaveDevice();

	UpdateData(FALSE);
}

void CH3CDlg::OnBnClickedCon()
{
	// TODO: 在此添加控件通知处理程序代码
	m_threadInfo.hWnd=this->m_hWnd;
	
	UpdateData();

	m_threadInfo.connect = !m_isConnect;
	m_threadInfo.username = tstrtostr(m_userName.GetString());
	m_threadInfo.password = tstrtostr(m_passWord.GetString());

	//if(m_threadInfo.pHWObj == NULL)
	//{
	//	// 新建华为对象
	//	string version = "2.40-0145";
	//	int ip[4] = {192,168,0,1};
	//	string devicename = "\\Device\\NPF_";
	//	devicename.append(m_threadInfo.device);
	//	m_threadInfo.pHWObj = new HuaweiNetwork(m_threadInfo.hWnd,
	//											m_threadInfo.username,
	//											m_threadInfo.password,
	//											devicename,
	//											ip,
	//											m_threadInfo.mac, 
	//											version);
	//}

	if(m_isConnect)
	{
		//m_isConnect=FALSE;
		//SetCtrls(TRUE);
		HANDLE result= CreateThread(NULL, 0, ConProc, (LPVOID)&m_threadInfo, 0, NULL);
		CloseHandle(result);
	}
	else
	{
		//m_isConnect=TRUE;
		SetCtrls(TRUE);
		static HANDLE result=NULL;
		DWORD exitCode;
		if(result!=NULL)
		{
			GetExitCodeThread(result,&exitCode);
			TerminateThread(result,exitCode);
		}
		result = CreateThread(NULL, 0, ConProc, (LPVOID)&m_threadInfo, 0, NULL);
		//CloseHandle(result);
	}
}

void CH3CDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	if(IDYES == MessageBox(_T("你确定退出吗？"), _T("确认"), MB_YESNO))
	{
		//NOTIFYICONDATA tnid; 
		//tnid.cbSize=sizeof(NOTIFYICONDATA); 
		//tnid.hWnd=this->m_hWnd; 
		//tnid.uID=IDR_MAINFRAME;
		SaveConfig();

		if(m_isConnect)
		{
			// 断开连接
			OnBnClickedCon();
		}

		Shell_NotifyIcon(NIM_DELETE,&tnd); 

		free(pAdapterInfo);
		delete m_threadInfo.pHWObj;

		CDialog::OnClose();
	}
}

int CH3CDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	m_threadInfo.hWnd=this->m_hWnd;
 
	tnd.cbSize=NOTIFYICONDATA_V2_SIZE; 
	tnd.hWnd=this->m_hWnd; 
	tnd.uID=IDR_MAINFRAME; 
	tnd.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP; 
	tnd.uCallbackMessage=WM_TRAY;
	tnd.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_DISCON)); 
	wcsncpy_s(tnd.szTip, _countof(tnd.szTip), _T("H3C 802.1x认证"), 128);
	Shell_NotifyIcon(NIM_ADD,&tnd);

	return 0;
}

LRESULT CH3CDlg::OnInfo(WPARAM wParam,LPARAM lParam)
{
	m_infoDisplay+=(char*)lParam;
	m_infoDisplay+="\r\n";
	if(!strcmp((char*)lParam,"登录成功！"))
	{
		m_isConnect = TRUE;
		SetNotify(m_isConnect);
	}

	if(!strcmp((char*)lParam,"连接断开")||!strcmp((char*)lParam,"连接失败..."))
	{
		m_isConnect = FALSE;
		SetCtrls(m_isConnect);
		SetNotify(m_isConnect);
	}
	
	UpdateData(FALSE);
	CEdit * edit=(CEdit *)GetDlgItem(IDC_EDIT1);
	edit->LineScroll(edit->GetLineCount());
	//((CEdit*)GetDlgItem(IDC_EDIT1))->Sc
	return 0;
}

LRESULT CH3CDlg::OnInterrupted(WPARAM, LPARAM)
{
	m_isConnect=FALSE;
	SetCtrls(m_isConnect);
	SetNotify(m_isConnect);

	//tnd.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_DISCON));
	//Shell_NotifyIcon(NIM_MODIFY,&tnd);
	//UpdateData(FALSE);
	return 0;
}

void CH3CDlg::OnBnClickedSave()
{}

void CH3CDlg::GetConfig()
{
	/*std::ifstream in("h3c.conf");
	string line;
	
	in >> line;
	m_userName.SetString(line.c_str());
	in >> line;
	m_passWord.SetString(line.c_str());
	in >> line;
	m_deviceNum = atoi(line.c_str());

	in.close();*/

	//IniFileProcessor processor(iniFilePath);
	Profile profile = profileManager->GetPresetting();

	m_userName = strtocstr(profile.username);
	m_passWord = strtocstr(profile.password);
	m_deviceUsed = profileManager->GetDevice();
	//m_deviceNum = atoi(profileManager->GetDevice().c_str());
}

void CH3CDlg::SaveConfig()
{
	UpdateData();
	
	// 读取信息
	//std::ofstream out("h3c.conf");
	//
	//out << m_userName << std::endl;
	//out << m_passWord << std::endl;
	//out << m_deviceNum << std::endl;

	//out.close();

	//IniFileProcessor processor(iniFilePath);

	//IniFileProcessor::IniMap map;
	//map["username"] = IniValue(string(m_userName.operator LPCSTR()));
	//map["password"] = IniValue(string(m_passWord.operator LPCSTR()));

	//processor.SetMap(map);
	//processor.Save();

	//string oldPresettingName = profileManager->GetPresettingName();
	SaveDevice();

	Profile profile;
	profile.username = string(cstrtostr(m_userName));
	profile.password = string(cstrtostr(m_passWord));

	if(profile.username.compare("") != 0)
	{
		profileManager->AddOrModify(profile);
		// presetting 的 username 改变了
		profileManager->SetPresetting(profile.username);
	}

	profileManager->Save();

	//m_userName.SetString((map["username"]).GetStrValue().c_str());
	//m_passWord.SetString((map["password"]).GetStrValue().c_str());
	//m_deviceNum = atoi((map["device"]).GetStrValue().c_str());
}

//BOOL CH3CDlg::PreCreateWindow(CREATESTRUCT& cs)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//cs.style=WS_POPUP; 
//cs.dwExStyle |=WS_EX_TOOLWINDOW; 
//
//	return CDialog::PreCreateWindow(cs);
//}

LRESULT CH3CDlg::OnTray(WPARAM wParam,LPARAM lParam)
{
	UINT uID=(UINT) wParam; 
	UINT uMouseMsg=(UINT) lParam; 
	CMenu menu;
	CMenu *pMenu;
	POINT pt;

	if(uMouseMsg==WM_LBUTTONDBLCLK)//如果是左键双击 
	{ 
		switch(uID) 
		{ 
		case IDR_MAINFRAME: 
		case IDI_DISCON:
			if(isShown==false) 
			{
				//显示主窗口 
				//ModifyStyle(WS_POPUP,WS_OVERLAPPEDWINDOW);  
				//ModifyStyleEx(WS_EX_TOOLWINDOW,WS_EX_TOPMOST); 
				ShowDialog();
			} 
			else 
			{
				HideDialog();
			}
			return 0;
			break; 
		default: 
			break; 
		} 
	} 

	if(uMouseMsg==WM_RBUTTONDOWN || uMouseMsg==WM_LBUTTONDOWN)//如果是单击左右键 
	{ 
		switch(uID) 
		{ 
		case IDR_MAINFRAME://如果是我们的图标 
		case IDI_DISCON:
			GetCursorPos(&pt);//取得鼠标位置 
			//执行相应操作 
			menu.LoadMenu(IDR_MENU1); 
			pMenu=menu.GetSubMenu(0); 
			ASSERT(pMenu!=0);
			::SetForegroundWindow(this->m_hWnd);
			pMenu->TrackPopupMenu (0,pt.x,pt.y,this); 
			break; 
		default: 
			break; 
		} 
	}

	return 0;

}

void CH3CDlg::ShowDialog()
{
	ShowWindow(SW_SHOWNORMAL); 
	isShown=true; 
}

void CH3CDlg::HideDialog()
{
	ShowWindow(SW_HIDE);//隐藏主窗口 
	isShown=false; 
}

void CH3CDlg::OnBnClickedHide()
{
	// TODO: 在此添加控件通知处理程序代码
	HideDialog();
}

void CH3CDlg::OnBnClickedExit()
{
	// TODO: 在此添加控件通知处理程序代码
	SendMessage(WM_CLOSE);
}

void CH3CDlg::OnMenuExit()
{
	// TODO: 在此添加命令处理程序代码
	SendMessage(WM_CLOSE);
}

void CH3CDlg::OnMenuMain()
{
	// TODO: 在此添加命令处理程序代码
	SendMessage(WM_TRAY,IDR_MAINFRAME,WM_LBUTTONDBLCLK);
}


void CH3CDlg::OnMenuCon()
{
	// TODO: 在此添加命令处理程序代码
	OnBnClickedCon();
}

void CH3CDlg::OnUpdateMenuCon(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	//pCmdUI->Enable(!m_isConnect);
	pCmdUI->SetText(m_isConnect ? _T("断开") : _T("连接"));
}

void CH3CDlg::OnBnClickedAbout()
{
	// TODO: 在此添加控件通知处理程序代码
	CAboutDlg dlg;
	dlg.DoModal();
}

void CH3CDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nID == SC_MINIMIZE)
	{
		HideDialog();
		return;
	}

	CDialog::OnSysCommand(nID, lParam);
}

void CH3CDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nIDEvent == 1)
	{
		KillTimer(nIDEvent);
		HideDialog();
	}

	CDialog::OnTimer(nIDEvent);
}

void CH3CDlg::OnBnClickedButtonSetting()
{
	// TODO: 在此添加控件通知处理程序代码
	CSettingDialog settingDialog(NULL, profileManager.get());
	settingDialog.DoModal();
	profileManager->Save();

	GetConfig();

	UpdateData(FALSE);
}

void CH3CDlg::SaveDevice(void)
{
	UpdateData();

	string strDeviceName = cstrtostr(m_textMac);

	profileManager->SetDevice(strDeviceName);
}

void CH3CDlg::SetCtrls(BOOL connected)
{
	if(connected)
		SetDlgItemText(ID_CON, _T("断开(&D)"));
	else
		SetDlgItemText(ID_CON, _T("连接(&C)"));

	((CEdit*)(this->GetDlgItem(IDC_USERNAME)))->SetReadOnly(connected);
	((CEdit*)(this->GetDlgItem(IDC_PASSWORD)))->SetReadOnly(connected);
	this->GetDlgItem(IDC_BUTTON_SETTING)->EnableWindow(!connected);
	this->GetDlgItem(IDC_COMBO1)->EnableWindow(!connected);
}

void CH3CDlg::SetNotify(BOOL connected)
{
	if(connected)
	{
		tnd.hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
		tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_INFO;
		tnd.uTimeout = 3000;
		tnd.dwInfoFlags = NIIF_INFO;
		wcsncpy_s(tnd.szInfoTitle, _countof(tnd.szInfoTitle), _T("连接状态"), 64);
		//strncpy_s(tnd.szInfoTitle, _countof(tnd.szInfoTitle), "连接状态", 64);
		wcsncpy_s(tnd.szInfo, _countof(tnd.szInfo), _T("已连接"), 64);
		//strncpy_s(tnd.szInfo, _countof(tnd.szInfo), "已连接", 64); 
		Shell_NotifyIcon(NIM_MODIFY, &tnd);
	}
	else
	{
		tnd.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_DISCON));
		tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_INFO;
		tnd.uTimeout = 3000;
		tnd.dwInfoFlags = NIIF_INFO;
		wcsncpy_s(tnd.szInfoTitle, _countof(tnd.szInfoTitle), _T("连接状态"), 64);
		//strncpy_s(tnd.szInfoTitle, _countof(tnd.szInfoTitle), "连接状态", 64);
		wcsncpy_s(tnd.szInfo, _countof(tnd.szInfo), _T("断开"), 64);
		//strncpy_s(tnd.szInfo, _countof(tnd.szInfo), "断开", 64); 
		Shell_NotifyIcon(NIM_MODIFY, &tnd);
	}
}

DWORD WINAPI CH3CDlg::ConProc(LPVOID lpParameter)
{
	pThreadInfo pThreadPara = (pThreadInfo)lpParameter;

	string version = "2.40-0145";
	int ip[4] = {192,168,0,1};
	string devicename = "\\Device\\NPF_";
	devicename.append(pThreadPara->device);
	HuaweiNetworkWin hwObj(pThreadPara->hWnd,
								pThreadPara->username,
								pThreadPara->password,
								devicename,
								ip,
								pThreadPara->mac, 
								version);

	if(pThreadPara->connect) // && pThreadPara->pHWObj != NULL)
	{
		//pThreadPara->pHWObj->Connect();
		hwObj.Connect();
	}
	else
	{
		//pThreadPara->pHWObj->DisConnect();
		hwObj.DisConnect();
	}
	return 0;
}
