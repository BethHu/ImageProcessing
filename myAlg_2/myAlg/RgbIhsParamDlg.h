#pragma once
#include "afxdialogex.h"
#include "resource.h"

class CRgbIhsParamDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRgbIhsParamDlg)

public:
	CRgbIhsParamDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRgbIhsParamDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_RGBIHS_OPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    BOOL m_bShowI;
    BOOL m_bShowH;
    BOOL m_bShowS;
    BOOL m_bShowRecon;
};
