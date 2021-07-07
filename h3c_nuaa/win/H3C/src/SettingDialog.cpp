// SettingDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"

#include <cstdlib>
#include <string>
#include <vector>
#include <map>

#include "H3C.h"
#include "SettingDialog.h"
#include "ProfileDialog.h"
#include "ProfileManager.h"
#include "CStringConvert.h"

using namespace std;

// CSettingDialog �Ի���

IMPLEMENT_DYNAMIC(CSettingDialog, CDialog)

CSettingDialog::CSettingDialog(CWnd* pParent /*=NULL*/, ProfileManager* profileManager /*=NULL*/)
	: CDialog(CSettingDialog::IDD, pParent)
	, profileManager(profileManager)
	, m_CurrentSelIndex(0)
{

}

CSettingDialog::~CSettingDialog()
{
}

void CSettingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROFILE, m_ListCtrProfile);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_buttonDelete);
	DDX_Control(pDX, IDC_BUTTON_MODIFY, m_buttonModify);
}


BEGIN_MESSAGE_MAP(CSettingDialog, CDialog)
//	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CSettingDialog::OnBnClickedButtonClose)
//ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CSettingDialog::OnBnClickedButtonRefresh)
//ON_NOTIFY(NM_DBLCLK, IDC_LIST_PROFILE, &CSettingDialog::OnNMDblclkListProfile)
ON_NOTIFY(NM_CLICK, IDC_LIST_PROFILE, &CSettingDialog::OnNMClickListProfile)
ON_BN_CLICKED(IDC_BUTTON_DELETE, &CSettingDialog::OnBnClickedButtonDelete)
ON_BN_CLICKED(IDC_BUTTON_ADD, &CSettingDialog::OnBnClickedButtonAdd)
ON_BN_CLICKED(IDC_BUTTON_MODIFY, &CSettingDialog::OnBnClickedButtonModify)
ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CSettingDialog::OnBnClickedButtonClose)
//ON_NOTIFY(HDN_ITEMCHANGED, 0, &CSettingDialog::OnHdnItemchangedListProfile)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PROFILE, &CSettingDialog::OnLvnItemchangedListProfile)
END_MESSAGE_MAP()


// CSettingDialog ��Ϣ�������
BOOL CSettingDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	DWORD dwStyle = m_ListCtrProfile.GetStyle();
	dwStyle |= LVS_EX_CHECKBOXES;
	m_ListCtrProfile.SetExtendedStyle(dwStyle);

	// ������Ŀ
	LV_COLUMN listCol;
	//char* arCols[4] = {"����", "ѡ��"};
	vector<tstring> colHeaders(1);
	colHeaders[0] = _T("����");
	listCol.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;

	vector<tstring>::iterator itr = colHeaders.begin();
	int nCol = 0;
	for(; itr != colHeaders.end(); ++itr)
	{
		listCol.iSubItem = nCol;
		listCol.pszText = (LPTSTR)colHeaders[nCol].c_str();
		listCol.fmt = LVCFMT_LEFT;
		m_ListCtrProfile.InsertColumn(nCol, &listCol);
		++nCol;
	}

	m_ListCtrProfile.SetColumnWidth(0, 300);

	FillProfiles();
	m_ListCtrProfile.SetCheck(m_CurrentProfileIndex);

	SetButtons(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CSettingDialog::FillProfiles(void)
{
	if(profileManager)
	{
		// profileManager �����Ѿ���ʼ��
		m_ListCtrProfile.DeleteAllItems();

		string presetting = profileManager->GetPresettingName();

		ProfileManager::ProfileMap profiles = profileManager->GetProfiles();
		ProfileManager::ProfileMap::iterator itr = profiles.begin();
		int nItem = 0, nIndex;
		for(; itr != profiles.end(); ++itr)
		{
			Profile profile((*itr).second);
			nIndex = m_ListCtrProfile.InsertItem(nItem, strtotstr(profile.username).c_str());
			if(profile.username.compare(presetting) == 0)
			{
				m_CurrentProfileIndex = nIndex;
			}
	
			++nItem;
		}
	}
}

BOOL CSettingDialog::SelectProfile(int index)
{
	if(index >= 0)
	{
		string selectedProfile = cstrtostr(m_ListCtrProfile.GetItemText(index, 0));
		
		if(selectedProfile.compare(profileManager->GetPresettingName()) != 0)
		{
			// ����ԭ��ѡ���
			profileManager->SetPresetting(selectedProfile);

			AfterModified();
			return TRUE;
		}
	}

	return FALSE;
}

void CSettingDialog::SetButtons(BOOL enabled)
{
	m_buttonModify.EnableWindow(enabled);
	m_buttonDelete.EnableWindow(enabled);
}

void CSettingDialog::OnNMClickListProfile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	m_CurrentSelIndex = pNMItemActivate->iItem;

	if(m_CurrentSelIndex >= 0)
	{
		SetButtons(TRUE);
	}
	else
	{
		SetButtons(FALSE);
	}

	*pResult = 0;
}

void CSettingDialog::OnBnClickedButtonDelete()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_CurrentSelIndex >= 0)
	{
		if(IDYES == MessageBox(_T("ȷ��ɾ�������ã�"), _T("ȷ��"), MB_YESNO))
		{
			string profileName = cstrtostr(m_ListCtrProfile.GetItemText(m_CurrentSelIndex, 0));
			profileManager->Delete(profileName);

			AfterModified();
		}
	}
}

void CSettingDialog::OnBnClickedButtonAdd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CProfileDialog profileDialog;
	if(IDOK == profileDialog.DoModal())
	{
		Profile profile = profileDialog.GetProfile();
		if(profile.username.compare("") != 0)
		{
			profileManager->AddOrModify(profile);
		}

		AfterModified();
	}
}

void CSettingDialog::OnBnClickedButtonModify()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_CurrentSelIndex >= 0)
	{
		string profileName = cstrtostr(m_ListCtrProfile.GetItemText(m_CurrentSelIndex, 0));
		ProfileManager::ProfileMap profiles = profileManager->GetProfiles();
		Profile oldProfile = profiles[profileName];
		CProfileDialog profileDialog(NULL, oldProfile);
		if(IDOK == profileDialog.DoModal())
		{
			Profile profile = profileDialog.GetProfile();
			if(profile.username.compare("") != 0)
			{
				profileManager->AddOrModify(profile);
			}
			if(profile.username.compare(oldProfile.username) != 0 && 
				profiles.find(profile.username) == profiles.end())
			{
				// ȫ�µ����ã����ϵ�ɾ��
				profileManager->Delete(oldProfile.username);
			}

			AfterModified();
		}
	}
}

void CSettingDialog::AfterModified(void)
{
	FillProfiles();
	m_ListCtrProfile.SetCheck(m_CurrentProfileIndex);

	m_CurrentSelIndex = -1;
	SetButtons(FALSE);
}

void CSettingDialog::OnBnClickedButtonClose()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SendMessage(WM_CLOSE);
}

void CSettingDialog::OnLvnItemchangedListProfile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int iCheck = pNMLV->iItem;
	if(m_ListCtrProfile.GetCheck(iCheck))
	{
		// ѡ�е�
		m_CurrentSelIndex = iCheck;
		if(SelectProfile(m_CurrentSelIndex))
		{
			// ������ѡ��
			int count = m_ListCtrProfile.GetItemCount();
			for(int i = 0; i < count; ++i)
			{
				if(i != m_CurrentProfileIndex)
					m_ListCtrProfile.SetCheck(i, FALSE);
				else
					m_ListCtrProfile.SetCheck(m_CurrentProfileIndex);
			}
		}
		else
		{
			// û���޸�ѡ��
			return;
		}
	}
	else
	{
		// ����˹�����ȡ����ѡ��
		if(iCheck == m_CurrentProfileIndex)
		{
			m_ListCtrProfile.SetCheck(iCheck, TRUE);
			SetButtons(FALSE);
		}
	}

	*pResult = 0;
}
