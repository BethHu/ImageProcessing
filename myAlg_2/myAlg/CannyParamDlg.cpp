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
    // DDV_MinMaxDouble(pDX, m_dSigma, 0.1, 10.0); // Moved to OnOK
    DDX_Text(pDX, IDC_EDIT_CANNY_LOW, m_dLowThresh);
    // DDV_MinMaxDouble(pDX, m_dLowThresh, 0.0, 2000.0); // Moved to OnOK
    DDX_Text(pDX, IDC_EDIT_CANNY_HIGH, m_dHighThresh);
    // DDV_MinMaxDouble(pDX, m_dHighThresh, 0.0, 2000.0); // Moved to OnOK
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
        AfxMessageBox(strErrors);
        return;
    }

    if (m_pImgIn == NULL || m_pImgIn->empty())
    {
        AfxMessageBox(_T("No input image for preview!"));
        return;
    }

    CImageDataset imgOut;
    if (CImageProcessingEx::cannyEdgeDetection(*m_pImgIn, imgOut, m_dSigma, m_dLowThresh, m_dHighThresh))
    {
        CString strTitle;
        strTitle.Format(_T("Preview: Canny (Sigma=%.1f, L=%.1f, H=%.1f)"), m_dSigma, m_dLowThresh, m_dHighThresh);
        // Use GetParent() to ensure the preview window is owned by the main window, not the modal dialog
        // But CImageDisplay::show implementation details matter. Assuming it works.
        CImageDisplay::show(imgOut, NULL, strTitle, 1, 1, 1, 0); 
    }
    else
    {
        AfxMessageBox(_T("Preview failed!"));
    }
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
