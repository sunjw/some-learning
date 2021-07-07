// ProfileDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "H3C.h"
#include "ProfileDialog.h"
#include "CStringConvert.h"

// CProfileDialog �Ի���

IMPLEMENT_DYNAMIC(CProfileDialog, CDialog)

CProfileDialog::CProfileDialog(CWnd* pParent /*=NULL*/, const Profile profile /*=Profile()*/)
	: CDialog(CProfileDialog::IDD, pParent)
	, m_Username(_T(""))
	, m_Password(_T(""))
	, m_Profile(profile)
{

}

CProfileDialog::~CProfileDialog()
{
}

void CProfileDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_Username);
	DDX_Text(pDX, IDC_EDIT2, m_Password);
}


BEGIN_MESSAGE_MAP(CProfileDialog, CDialog)
//	ON_BN_CLICKED(IDOK, &CProfileDialog::OnBnClickedOk)
ON_BN_CLICKED(IDOK, &CProfileDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CProfileDialog ��Ϣ�������

BOOL CProfileDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_Username = strtocstr(m_Profile.username);
	m_Password = strtocstr(m_Profile.password);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CProfileDialog::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	OnOK();
}

Profile CProfileDialog::GetProfile(void)
{
	string username = cstrtostr(m_Username);
	string password = cstrtostr(m_Password);
	return Profile(username, password);
}
