#include "stdafx.h"
#include "myAlg.h"
#include "CannyParamDlg.h"
#include "afxdialogex.h"
#include "ImageProcessingEx.h"

IMPLEMENT_DYNAMIC(CCannyParamDlg, CDialogEx)

CCannyParamDlg::CCannyParamDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCannyParamDlg::IDD, pParent)
	, m_dSigma(1.0)
	, m_dLowThresh(50.0)
	, m_dHighThresh(150.0)
	, m_pImgIn(NULL)
{
}

CCannyParamDlg::~CCannyParamDlg()
{
}

void CCannyParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CANNY_SIGMA, m_dSigma);
	DDX_Text(pDX, IDC_EDIT_CANNY_LOW, m_dLowThresh);
	DDX_Text(pDX, IDC_EDIT_CANNY_HIGH, m_dHighThresh);
}

BEGIN_MESSAGE_MAP(CCannyParamDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW, &CCannyParamDlg::OnBnClickedButtonPreview)
END_MESSAGE_MAP()

BOOL CCannyParamDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	return TRUE;
}

void CCannyParamDlg::OnBnClickedButtonPreview()
{
	if (!UpdateData(TRUE)) return;

	CString strErrors;
	if (m_dSigma < 0.1 || m_dSigma > 10.0) strErrors += _T("Gaussian Sigma must be between 0.1 and 10.0.\n");
	if (m_dLowThresh < 0.0 || m_dLowThresh > 2000.0) strErrors += _T("Low Threshold must be between 0.0 and 2000.0.\n");
	if (m_dHighThresh < 0.0 || m_dHighThresh > 2000.0) strErrors += _T("High Threshold must be between 0.0 and 2000.0.\n");
	if (m_dLowThresh > m_dHighThresh) strErrors += _T("Low Threshold cannot be greater than High Threshold.\n");

	if (!strErrors.IsEmpty())
	{
		// 静默返回，不弹提示
		return;
	}

	if (m_pImgIn == NULL || m_pImgIn->empty())
	{
		// 无预览图像则静默返回
		return;
	}

	CImageDataset imgOut;
	if (CImageProcessingEx::cannyEdgeDetection(*m_pImgIn, imgOut, m_dSigma, m_dLowThresh, m_dHighThresh))
	{
		CString strTitle;
		strTitle.Format(_T("Preview: Canny (Sigma=%.1f, L=%.1f, H=%.1f)"), m_dSigma, m_dLowThresh, m_dHighThresh);
		CImageDisplay::show(imgOut, NULL, strTitle, 1, 1, 1, 0);
	}
	// 预览失败静默忽略
}

void CCannyParamDlg::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	CString strErrors;
	if (m_dSigma < 0.1 || m_dSigma > 10.0)
		strErrors += _T("Gaussian Sigma must be between 0.1 and 10.0.\n");
	if (m_dLowThresh < 0.0 || m_dLowThresh > 2000.0)
		strErrors += _T("Low Threshold must be between 0.0 and 2000.0.\n");
	if (m_dHighThresh < 0.0 || m_dHighThresh > 2000.0)
		strErrors += _T("High Threshold must be between 0.0 and 2000.0.\n");
	if (m_dLowThresh > m_dHighThresh)
		strErrors += _T("Low Threshold cannot be greater than High Threshold.\n");

	if (!strErrors.IsEmpty())
	{
		AfxMessageBox(strErrors);
		return;
	}

	CDialogEx::OnOK();
}
