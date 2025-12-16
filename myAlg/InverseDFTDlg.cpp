// InverseDFTDlg.cpp
#include "stdafx.h"
#include "myAlg.h"
#include "InverseDFTDlg.h"
#include "afxdialogex.h"
#include "FourierTransform.h"
#include "ImageProcessingEx.h"

IMPLEMENT_DYNAMIC(CInverseDFTDlg, CDialogEx)

CInverseDFTDlg::CInverseDFTDlg(CFourierSpectrumDlg* pParentDlg, CWnd* pParent)
    : CDialogEx(IDD_INVERSEDFTDLG, pParent)
    , m_pParentDlg(pParentDlg)
    , m_mse(0.0)
    , m_psnr(0.0)
    , m_maxError(0.0)
{
}

CInverseDFTDlg::~CInverseDFTDlg()
{
}

void CInverseDFTDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CInverseDFTDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_INVERSE, &CInverseDFTDlg::OnBnClickedButtonInverse)
    ON_BN_CLICKED(IDC_BUTTON_ERROR, &CInverseDFTDlg::OnBnClickedButtonError)
    ON_BN_CLICKED(IDC_BUTTON_COMPARE, &CInverseDFTDlg::OnBnClickedButtonCompare)
END_MESSAGE_MAP()

BOOL CInverseDFTDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    
    SetWindowText(_T("傅里叶反变换与误差分析"));
    
    // 初始禁用误差分析按钮，直到完成反变换
    GetDlgItem(IDC_BUTTON_ERROR)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_COMPARE)->EnableWindow(FALSE);
    
    return TRUE;
}

void CInverseDFTDlg::OnBnClickedButtonInverse()
{
	if (!m_pParentDlg)
	{
		AfxMessageBox(_T("父对话框无效"));
		return;
	}
	
	// 调用父对话框的反变换计算
	if (m_pParentDlg->ComputeInverseDFT())
	{
		// 反变换成功，启用误差分析按钮
		GetDlgItem(IDC_BUTTON_ERROR)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_COMPARE)->EnableWindow(TRUE);
	}
	else
	{
		AfxMessageBox(_T("傅里叶反变换失败"));
	}
}

// 修改OnBnClickedButtonError函数
void CInverseDFTDlg::OnBnClickedButtonError()
{
	if (!m_pParentDlg)
	{
		AfxMessageBox(_T("父对话框无效"));
		return;
	}
	
	// 调用父对话框的误差分析
	m_pParentDlg->ShowErrorAnalysis();
	
	// 获取误差结果并在对话框中显示
	m_mse = m_pParentDlg->GetMSE();
	m_psnr = m_pParentDlg->GetPSNR();
	m_maxError = m_pParentDlg->GetMaxError();
	
	// 如果有静态文本控件显示误差值，可以在这里更新
	// CString strMSE;
	// strMSE.Format(_T("%.6f"), m_mse);
	// SetDlgItemText(IDC_STATIC_MSE, strMSE);
	// ... 类似处理PSNR和最大误差
}

// 修改OnBnClickedButtonCompare函数
void CInverseDFTDlg::OnBnClickedButtonCompare()
{
	if (!m_pParentDlg)
	{
		AfxMessageBox(_T("父对话框无效"));
		return;
	}
	
	// 同时显示原图、重建图和误差图进行比较
	CImageDataset* pOriginal = m_pParentDlg->GetOriginalImage();
	
	if (pOriginal && !pOriginal->empty())
	{
		CImageDisplay::show(*pOriginal, this, _T("原始图像"), 1, 1, 1, 0);
	}
	
	// 获取重建图像
	CImageDataset& reconstructed = m_pParentDlg->GetReconstructedImage();
	if (!reconstructed.empty())
	{
		CImageDisplay::show(reconstructed, this, _T("重建图像"), 1, 1, 1, 0);
	}
	
	// 获取误差图像
	CImageDataset& errorImg = m_pParentDlg->GetErrorImage();
	if (!errorImg.empty())
	{
		CImageDisplay::show(errorImg, this, _T("误差图像"), 1, 1, 1, 0);
	}
	
	// 显示误差统计
	if (m_mse > 0 || m_psnr > 0)
	{
		CString msg;
		msg.Format(_T("比较分析：\n\n")
				   _T("MSE: %.6f\n")
				   _T("PSNR: %.2f dB\n")
				   _T("最大误差: %.6f"),
				   m_mse, m_psnr, m_maxError);
		AfxMessageBox(msg, MB_OK | MB_ICONINFORMATION);
	}
}