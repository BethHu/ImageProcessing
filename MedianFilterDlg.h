#pragma once
#include "afxdialogex.h"

// CMedianFilterDlg 对话框
class CMedianFilterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMedianFilterDlg)

public:
	CMedianFilterDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMedianFilterDlg();

	// 对话框数据
	enum { IDD = IDD_MEDIANFILTERDLG };

	int m_windowSize;  // 窗口大小

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedButtonSure();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnEnChangeEditwindow();
};