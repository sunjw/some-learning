#pragma once

#include "ProfileManager.h"

// CProfileDialog 对话框

class CProfileDialog : public CDialog
{
	DECLARE_DYNAMIC(CProfileDialog)

public:
	CProfileDialog(CWnd* pParent = NULL, const Profile profile = Profile());   // 标准构造函数
	virtual ~CProfileDialog();

// 对话框数据
	enum { IDD = IDD_PROFILE_DIALOG };

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

	Profile GetProfile(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CString m_Username;
	CString m_Password;
	Profile m_Profile;
};
