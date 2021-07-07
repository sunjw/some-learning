#pragma once

#include "ProfileManager.h"

// CProfileDialog �Ի���

class CProfileDialog : public CDialog
{
	DECLARE_DYNAMIC(CProfileDialog)

public:
	CProfileDialog(CWnd* pParent = NULL, const Profile profile = Profile());   // ��׼���캯��
	virtual ~CProfileDialog();

// �Ի�������
	enum { IDD = IDD_PROFILE_DIALOG };

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

	Profile GetProfile(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	CString m_Username;
	CString m_Password;
	Profile m_Profile;
};
