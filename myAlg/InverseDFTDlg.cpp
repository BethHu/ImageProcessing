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
    , m_pFilteredDFT(nullptr)
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
    
    // 如果有过滤后的 DFT 数据，使用过滤后的数据进行反变换
    if (m_pFilteredDFT && !m_pFilteredDFT->empty())
    {
        // 使用过滤后的 DFT 进行反变换
        TRACE(_T("使用过滤后的DFT数据进行反变换\n"));
        
        if (CFourierTransform::IDFT2D(*m_pFilteredDFT, m_reconstructedImage, 
                                     m_pParentDlg->m_dftWidth, m_pParentDlg->m_dftHeight))
        {
            // 重要：将重建结果也保存到父对话框中，以便后续显示
            CImageDataset& parentReconstructed = m_pParentDlg->GetReconstructedImage();
            m_reconstructedImage.duplicate(parentReconstructed);
            
            TRACE(_T("过滤后的反变换成功，重建图像大小=%d\n"), 
                  (int)m_reconstructedImage.m_xsize);
            
            GetDlgItem(IDC_BUTTON_ERROR)->EnableWindow(TRUE);
            GetDlgItem(IDC_BUTTON_COMPARE)->EnableWindow(TRUE);
            AfxMessageBox(_T("反变换成功（使用过滤后的数据）"));
        }
        else
        {
            AfxMessageBox(_T("反变换失败"));
        }
    }
    else
    {
        // 如果没有过滤后的数据，使用原始 DFT 进行反变换
        TRACE(_T("使用原始DFT数据进行反变换\n"));
        
        if (m_pParentDlg->ComputeInverseDFT())
        {
            GetDlgItem(IDC_BUTTON_ERROR)->EnableWindow(TRUE);
            GetDlgItem(IDC_BUTTON_COMPARE)->EnableWindow(TRUE);
            AfxMessageBox(_T("反变换成功"));
        }
        else
        {
            AfxMessageBox(_T("反变换失败"));
        }
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
	
	// 首先显示原始图像
	CImageDataset* pOriginal = m_pParentDlg->GetOriginalImage();
	
	if (pOriginal && !pOriginal->empty())
	{
		TRACE(_T("显示原始图像，尺寸：%dx%d\n"), pOriginal->m_xsize, pOriginal->m_ysize);
		CImageDisplay::show(*pOriginal, this, _T("原始图像"), 1, 1, 1, 0);
	}
	else
	{
		TRACE(_T("警告：原始图像为空或无效\n"));
		AfxMessageBox(_T("原始图像无效或未加载！"));
	}
	
	// 获取重建图像 - 首先尝试从本对话框的成员（过滤后反变换的结果）
	CImageDataset* pReconstructed = nullptr;
	
	// 优先使用本对话框中的重建图像（用于理想滤波器工作流）
	if (!m_reconstructedImage.empty())
	{
		pReconstructed = &m_reconstructedImage;
		TRACE(_T("使用本对话框的重建图像\n"));
	}
	else
	{
		// 其次使用父对话框中的重建图像
		CImageDataset& reconstructed = m_pParentDlg->GetReconstructedImage();
		if (!reconstructed.empty())
		{
			pReconstructed = &reconstructed;
			TRACE(_T("使用父对话框的重建图像\n"));
		}
	}
	
	if (pReconstructed)
	{
		TRACE(_T("显示重建图像，尺寸：%dx%d\n"), pReconstructed->m_xsize, pReconstructed->m_ysize);
		CImageDisplay::show(*pReconstructed, this, _T("重建图像"), 1, 1, 1, 0);
	}
	else
	{
		AfxMessageBox(_T("重建图像为空！请先点击'反变换'按钮。"));
		return;
	}
	
	// 获取误差图像
	CImageDataset& errorImg = m_pParentDlg->GetErrorImage();
	if (!errorImg.empty())
	{
		TRACE(_T("显示误差图像，尺寸：%dx%d\n"), errorImg.m_xsize, errorImg.m_ysize);
		CImageDisplay::show(errorImg, this, _T("误差图像"), 1, 1, 1, 0);
	}
	
	// 显示误差统计
	double mse = m_pParentDlg->GetMSE();
	double psnr = m_pParentDlg->GetPSNR();
	double maxError = m_pParentDlg->GetMaxError();
	
	if (mse >= 0 || psnr >= 0)
	{
		CString msg;
		msg.Format(_T("比较分析：\n\n")
				   _T("MSE: %.6f\n")
				   _T("PSNR: %.2f dB\n")
				   _T("最大误差: %.6f"),
				   mse, psnr, maxError);
		AfxMessageBox(msg, MB_OK | MB_ICONINFORMATION);
	}
}