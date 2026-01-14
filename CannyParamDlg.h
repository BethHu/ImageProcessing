#pragma once

#include "afxwin.h"
#include "resource.h"
#include "RSDIPLib.h"

#ifndef IDD_DIALOG_CANNY
#define IDD_DIALOG_CANNY 135
#endif

// 控件 ID 兜底
#ifndef IDC_EDIT_CANNY_SIGMA
#define IDC_EDIT_CANNY_SIGMA 1031
#endif
#ifndef IDC_EDIT_CANNY_LOW
#define IDC_EDIT_CANNY_LOW 1032
#endif
#ifndef IDC_EDIT_CANNY_HIGH
#define IDC_EDIT_CANNY_HIGH 1033
#endif

class CCannyParamDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCannyParamDlg)

public:
	CCannyParamDlg(CWnd* pParent = NULL);
	virtual ~CCannyParamDlg();

	enum { IDD = IDD_DIALOG_CANNY };

	double m_dSigma;
	double m_dLowThresh;
	double m_dHighThresh;
	CImageDataset* m_pImgIn;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedButtonPreview();
};
