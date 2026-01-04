#pragma once
#include "afxdialogex.h"

class CImageDataset;

class CMedianParamDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CMedianParamDlg)

public:
    CMedianParamDlg(CWnd* pParent = nullptr);
    virtual ~CMedianParamDlg();
    enum { IDD = IDD_DIALOG_MEDIAN }; 

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()

public:
    CImageDataset* m_pImgIn;
    int m_nWinSize; // 定义窗口大小

    virtual void OnOK();                    //确定按钮
    afx_msg void OnBnClickedButtonPreview();//预览按钮

};