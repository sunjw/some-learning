#pragma once
#include "afxcmn.h"

#include "ProfileManager.h"
#include "afxwin.h"

// CSettingDialog 对话框

class CSettingDialog : public CDialog
{
	DECLARE_DYNAMIC(CSettingDialog)

public:
	CSettingDialog(CWnd* pParent = NULL, ProfileManager* profileManager = NULL);
	virtual ~CSettingDialog();

// 对话框数据
	enum { IDD = IDD_SETTING_DIALOG };

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnNMClickListProfile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListProfile(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	void SetButtons(BOOL enabled);
	BOOL SelectProfile(int index);
	void AfterModified(void);
	void FillProfiles(void);
	
	ProfileManager* profileManager;
	CListCtrl m_ListCtrProfile;
	CButton m_buttonDelete;
	CButton m_buttonModify;
	int m_CurrentSelIndex;
	int m_CurrentProfileIndex;
	
};
