#pragma once
#include "afxwin.h"
#include "resource.h"
#include "RSDIPLib.h"

class CCannyParamDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CCannyParamDlg)

public:
    CCannyParamDlg(CWnd* pParent = NULL);   
    virtual ~CCannyParamDlg();

// Dialog Data
    enum { IDD = IDD_DIALOG_CANNY };

    double m_dSigma;
    double m_dLowThresh;
    double m_dHighThresh;
    CImageDataset* m_pImgIn; // ‘§¿¿

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnBnClickedButtonPreview();
};
