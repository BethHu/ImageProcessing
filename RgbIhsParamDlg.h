#pragma once

#include "afxdialogex.h"
#include "resource.h"

#ifndef IDD_DIALOG_RGBIHS_OPT
#define IDD_DIALOG_RGBIHS_OPT 134
#endif

// Fallback control IDs in case resource.h is not seen
#ifndef IDC_CHECK_SHOW_I
#define IDC_CHECK_SHOW_I 1026
#endif
#ifndef IDC_CHECK_SHOW_H
#define IDC_CHECK_SHOW_H 1027
#endif
#ifndef IDC_CHECK_SHOW_S
#define IDC_CHECK_SHOW_S 1028
#endif
#ifndef IDC_CHECK_SHOW_RECON
#define IDC_CHECK_SHOW_RECON 1029
#endif

class CRgbIhsParamDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRgbIhsParamDlg)

public:
	CRgbIhsParamDlg(CWnd* pParent = NULL);
	virtual ~CRgbIhsParamDlg();

	enum { IDD = IDD_DIALOG_RGBIHS_OPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

public:
	BOOL m_bShowI;
	BOOL m_bShowH;
	BOOL m_bShowS;
	BOOL m_bShowRecon;
};
