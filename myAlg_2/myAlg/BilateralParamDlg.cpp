// BilateralParamDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "myAlg.h"
#include "BilateralParamDlg.h"
#include "afxdialogex.h"
#include "ImageProcessingEx.h"

// CBilateralParamDlg 对话框
IMPLEMENT_DYNAMIC(CBilateralParamDlg, CDialogEx)

// 构造函数：
CBilateralParamDlg::CBilateralParamDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CBilateralParamDlg::IDD, pParent)
    , m_nD(5)          // 默认窗口大小d=5
    , m_nSigmaS(3)     // 默认σₛ=3
    , m_dSigmaR(0.1)   // 默认σᵣ=0.1
    , m_pImgIn(NULL)
{
}

CBilateralParamDlg::~CBilateralParamDlg()
{
}

// 数据交换：补充参数范围校验
void CBilateralParamDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    // 绑定窗口大小d
    DDX_Text(pDX, IDC_EDIT_BILAT_D, m_nD);
    // DDV_MinMaxInt(pDX, m_nD, 3, 15); // Moved to OnOK

    // 绑定空间域σₛ
    DDX_Text(pDX, IDC_EDIT_BILAT_SIGMAS, m_nSigmaS);
    // DDV_MinMaxInt(pDX, m_nSigmaS, 1, 100); // Moved to OnOK

    // 绑定灰度域σᵣ
    DDX_Text(pDX, IDC_EDIT_BILAT_SIGMAR, m_dSigmaR);
    // DDV_MinMaxDouble(pDX, m_dSigmaR, 0.05, 100.0); // Moved to OnOK
}

BEGIN_MESSAGE_MAP(CBilateralParamDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_PREVIEW, &CBilateralParamDlg::OnBnClickedButtonPreview)
END_MESSAGE_MAP()

// 对话框初始化：显示默认参数
BOOL CBilateralParamDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();  // 调用父类初始化（必须）

    // 设置编辑框默认值（与构造函数默认参数一致）
    // SetDlgItemInt(IDC_EDIT_BILAT_D, m_nD);          // 显示d=5
    // SetDlgItemInt(IDC_EDIT_BILAT_SIGMAS, m_nSigmaS); // 显示σₛ=3
    // SetDlgItemText(IDC_EDIT_BILAT_SIGMAR, _T("0.1"));// 显示σᵣ=0.1（double转字符串）

    return TRUE;  // MFC要求返回TRUE，否则对话框无法正常显示
}

void CBilateralParamDlg::OnBnClickedButtonPreview()
{
    if (!UpdateData(TRUE)) return;

    CString strErrors;
    if (m_nD < 3 || m_nD > 15) strErrors += _T("Window size d must be between 3 and 15.\n");
    if (m_nD % 2 == 0) strErrors += _T("Window size d must be odd.\n");
    if (m_nSigmaS < 1 || m_nSigmaS > 100) strErrors += _T("SigmaS must be between 1 and 100.\n");
    if (m_dSigmaR < 0.05 || m_dSigmaR > 100.0) strErrors += _T("SigmaR must be between 0.05 and 100.0.\n");

    if (!strErrors.IsEmpty())
    {
        AfxMessageBox(strErrors);
        return;
    }

    if (m_pImgIn == NULL || m_pImgIn->empty())
    {
        AfxMessageBox(_T("No input image for preview!"));
        return;
    }

    CImageDataset imgOut;
    if (CImageProcessingEx::bilateralFilter(*m_pImgIn, imgOut, m_nD, m_nSigmaS, m_dSigmaR))
    {
        CString strTitle;
        strTitle.Format(_T("Preview: Bilateral (d=%d, SigmaS=%d, SigmaR=%.2f)"), m_nD, m_nSigmaS, m_dSigmaR);
        
        if (imgOut.m_rastercount >= 3)
            CImageDisplay::show(imgOut, NULL, strTitle, 1, 2, 3, 0);
        else
            CImageDisplay::show(imgOut, NULL, strTitle, 1, 1, 1, 0);
    }
    else
    {
        AfxMessageBox(_T("Preview failed!"));
    }
}

// “确定”按钮响应：校验窗口大小d为奇数
void CBilateralParamDlg::OnOK()
{
    // 1. 获取编辑框输入值（更新成员变量）
    if (!UpdateData(TRUE)) return;

    CString strErrors;
    if (m_nD < 3 || m_nD > 15) strErrors += _T("Window size d must be between 3 and 15.\n");
    if (m_nD % 2 == 0) strErrors += _T("Window size d must be odd.\n");
    if (m_nSigmaS < 1 || m_nSigmaS > 100) strErrors += _T("SigmaS must be between 1 and 100.\n");
    if (m_dSigmaR < 0.05 || m_dSigmaR > 100.0) strErrors += _T("SigmaR must be between 0.05 and 100.0.\n");

    if (!strErrors.IsEmpty())
    {
        AfxMessageBox(strErrors);
        return;
    }

    // 3. 参数全部合法，关闭对话框
    CDialogEx::OnOK();
}