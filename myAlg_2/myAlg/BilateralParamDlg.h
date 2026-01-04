#pragma once
#include "afxdialogex.h"

class CImageDataset;

// CBilateralParamDlg 对话框（支持双边滤波参数输入与校验）
class CBilateralParamDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CBilateralParamDlg)

public:
    CBilateralParamDlg(CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CBilateralParamDlg();

// 对话框数据
    enum { IDD = IDD_DIALOG_BILA };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    // 双边滤波核心参数（与编辑框绑定）
    int m_nD;          // 窗口大小d（推荐默认5，范围3~9，奇数）
    int m_nSigmaS;     // 空间域σₛ（推荐默认3，范围1~10）
    double m_dSigmaR;  // 灰度域σᵣ（推荐默认0.1，范围0.05~0.2）

    CImageDataset* m_pImgIn;

    // 补充：对话框初始化（显示默认参数）
    virtual BOOL OnInitDialog();
    // 补充：“确定”按钮响应（校验d为奇数）
    virtual void OnOK();
    afx_msg void OnBnClickedButtonPreview();
};