// H3CDlg.cpp : ʵ���ļ�
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


// CH3CDlg �Ի���

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


// CH3CDlg ��Ϣ�������

BOOL CH3CDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	CreateMutex(NULL, true, _T("Huawei"));
	if(GetLastError() == ERROR_ALREADY_EXISTS)   
	{   
		MessageBoxEx(NULL, 
			_T("�Ѿ���һ��ʵ�������У�"),
			_T("H3C 802.1x"),
			MB_OK|MB_APPLMODAL|MB_ICONEXCLAMATION,
			0x0804);
		Shell_NotifyIcon(NIM_DELETE,&tnd);
		PostQuitMessage(0);
		return FALSE;   
	}

	// ��ȡ��Ϣ
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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

CStrVector CH3CDlg::ParseCmdLine()
{
	// �������л�ȡ�ļ�·��
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
	// �������л�ȡ�ļ�·������
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CH3CDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	GetMac(m_deviceNum);

	SaveDevice();

	UpdateData(FALSE);
}

void CH3CDlg::OnBnClickedCon()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_threadInfo.hWnd=this->m_hWnd;
	
	UpdateData();

	m_threadInfo.connect = !m_isConnect;
	m_threadInfo.username = tstrtostr(m_userName.GetString());
	m_threadInfo.password = tstrtostr(m_passWord.GetString());

	//if(m_threadInfo.pHWObj == NULL)
	//{
	//	// �½���Ϊ����
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	
	if(IDYES == MessageBox(_T("��ȷ���˳���"), _T("ȷ��"), MB_YESNO))
	{
		//NOTIFYICONDATA tnid; 
		//tnid.cbSize=sizeof(NOTIFYICONDATA); 
		//tnid.hWnd=this->m_hWnd; 
		//tnid.uID=IDR_MAINFRAME;
		SaveConfig();

		if(m_isConnect)
		{
			// �Ͽ�����
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

	// TODO:  �ڴ������ר�õĴ�������

	m_threadInfo.hWnd=this->m_hWnd;
 
	tnd.cbSize=NOTIFYICONDATA_V2_SIZE; 
	tnd.hWnd=this->m_hWnd; 
	tnd.uID=IDR_MAINFRAME; 
	tnd.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP; 
	tnd.uCallbackMessage=WM_TRAY;
	tnd.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_DISCON)); 
	wcsncpy_s(tnd.szTip, _countof(tnd.szTip), _T("H3C 802.1x��֤"), 128);
	Shell_NotifyIcon(NIM_ADD,&tnd);

	return 0;
}

LRESULT CH3CDlg::OnInfo(WPARAM wParam,LPARAM lParam)
{
	m_infoDisplay+=(char*)lParam;
	m_infoDisplay+="\r\n";
	if(!strcmp((char*)lParam,"��¼�ɹ���"))
	{
		m_isConnect = TRUE;
		SetNotify(m_isConnect);
	}

	if(!strcmp((char*)lParam,"���ӶϿ�")||!strcmp((char*)lParam,"����ʧ��..."))
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
	
	// ��ȡ��Ϣ
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
		// presetting �� username �ı���
		profileManager->SetPresetting(profile.username);
	}

	profileManager->Save();

	//m_userName.SetString((map["username"]).GetStrValue().c_str());
	//m_passWord.SetString((map["password"]).GetStrValue().c_str());
	//m_deviceNum = atoi((map["device"]).GetStrValue().c_str());
}

//BOOL CH3CDlg::PreCreateWindow(CREATESTRUCT& cs)
//{
//	// TODO: �ڴ����ר�ô����/����û���
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

	if(uMouseMsg==WM_LBUTTONDBLCLK)//��������˫�� 
	{ 
		switch(uID) 
		{ 
		case IDR_MAINFRAME: 
		case IDI_DISCON:
			if(isShown==false) 
			{
				//��ʾ������ 
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

	if(uMouseMsg==WM_RBUTTONDOWN || uMouseMsg==WM_LBUTTONDOWN)//����ǵ������Ҽ� 
	{ 
		switch(uID) 
		{ 
		case IDR_MAINFRAME://��������ǵ�ͼ�� 
		case IDI_DISCON:
			GetCursorPos(&pt);//ȡ�����λ�� 
			//ִ����Ӧ���� 
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
	ShowWindow(SW_HIDE);//���������� 
	isShown=false; 
}

void CH3CDlg::OnBnClickedHide()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	HideDialog();
}

void CH3CDlg::OnBnClickedExit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SendMessage(WM_CLOSE);
}

void CH3CDlg::OnMenuExit()
{
	// TODO: �ڴ���������������
	SendMessage(WM_CLOSE);
}

void CH3CDlg::OnMenuMain()
{
	// TODO: �ڴ���������������
	SendMessage(WM_TRAY,IDR_MAINFRAME,WM_LBUTTONDBLCLK);
}


void CH3CDlg::OnMenuCon()
{
	// TODO: �ڴ���������������
	OnBnClickedCon();
}

void CH3CDlg::OnUpdateMenuCon(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	//pCmdUI->Enable(!m_isConnect);
	pCmdUI->SetText(m_isConnect ? _T("�Ͽ�") : _T("����"));
}

void CH3CDlg::OnBnClickedAbout()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAboutDlg dlg;
	dlg.DoModal();
}

void CH3CDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(nID == SC_MINIMIZE)
	{
		HideDialog();
		return;
	}

	CDialog::OnSysCommand(nID, lParam);
}

void CH3CDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(nIDEvent == 1)
	{
		KillTimer(nIDEvent);
		HideDialog();
	}

	CDialog::OnTimer(nIDEvent);
}

void CH3CDlg::OnBnClickedButtonSetting()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		SetDlgItemText(ID_CON, _T("�Ͽ�(&D)"));
	else
		SetDlgItemText(ID_CON, _T("����(&C)"));

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
		wcsncpy_s(tnd.szInfoTitle, _countof(tnd.szInfoTitle), _T("����״̬"), 64);
		//strncpy_s(tnd.szInfoTitle, _countof(tnd.szInfoTitle), "����״̬", 64);
		wcsncpy_s(tnd.szInfo, _countof(tnd.szInfo), _T("������"), 64);
		//strncpy_s(tnd.szInfo, _countof(tnd.szInfo), "������", 64); 
		Shell_NotifyIcon(NIM_MODIFY, &tnd);
	}
	else
	{
		tnd.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_DISCON));
		tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_INFO;
		tnd.uTimeout = 3000;
		tnd.dwInfoFlags = NIIF_INFO;
		wcsncpy_s(tnd.szInfoTitle, _countof(tnd.szInfoTitle), _T("����״̬"), 64);
		//strncpy_s(tnd.szInfoTitle, _countof(tnd.szInfoTitle), "����״̬", 64);
		wcsncpy_s(tnd.szInfo, _countof(tnd.szInfo), _T("�Ͽ�"), 64);
		//strncpy_s(tnd.szInfo, _countof(tnd.szInfo), "�Ͽ�", 64); 
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
