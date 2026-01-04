#include "stdafx.h"
#include "myAlg.h"
#include "MedianParamDlg.h"
#include "afxdialogex.h"
#include "ImageProcessingEx.h"

IMPLEMENT_DYNAMIC(CMedianParamDlg, CDialogEx)

CMedianParamDlg::CMedianParamDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(CMedianParamDlg::IDD, pParent)
    , m_pImgIn(NULL)
    , m_nWinSize(0) //
{
}

CMedianParamDlg::~CMedianParamDlg()
{
}

void CMedianParamDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
  
    DDX_Text(pDX, IDC_EDIT_MEDIAN_WIN, m_nWinSize);
}

BEGIN_MESSAGE_MAP(CMedianParamDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_PREVIEW, &CMedianParamDlg::OnBnClickedButtonPreview)
END_MESSAGE_MAP()

void CMedianParamDlg::OnOK()
{
   
    UpdateData(TRUE);

    
    const int MIN_WIN = 3;
    const int MAX_WIN = 11;
    BOOL bValid = TRUE;
    CString strErr;
    
    if (m_nWinSize == 0)
    {
        strErr = _T("Please enter the filter window size!");
        bValid = FALSE;
    }

    else if (m_nWinSize < MIN_WIN || m_nWinSize > MAX_WIN)
    {
        strErr.Format(_T("Window size must be between %d and %d!"), MIN_WIN, MAX_WIN);
        bValid = FALSE;
    }

    else if (m_nWinSize % 2 == 0)
    {
        strErr = _T("Window size must be an odd number (e.g., 3/5/7)!");
        bValid = FALSE;
    }

    if (!bValid)
    {
        AfxMessageBox(strErr);

        GetDlgItem(IDC_EDIT_MEDIAN_WIN)->SetFocus();
        return; 
    }

    CDialogEx::OnOK();
}
void CMedianParamDlg::OnBnClickedButtonPreview()
{
    if (!UpdateData(TRUE)) return;

    const int MIN_WIN = 3;
    const int MAX_WIN = 11;
    CString strErr;

    if (m_nWinSize == 0)
    {
        strErr = _T("Window size cannot be 0.");
    }
    else if (m_nWinSize < MIN_WIN || m_nWinSize > MAX_WIN)
    {
        strErr.Format(_T("Window size must be between %d and %d."), MIN_WIN, MAX_WIN);
    }
    else if (m_nWinSize % 2 == 0)
    {
        strErr = _T("Window size must be odd (e.g., 3, 5, 7).");
    }

    if (!strErr.IsEmpty())
    {
        AfxMessageBox(strErr);
        GetDlgItem(IDC_EDIT_MEDIAN_WIN)->SetFocus();
        return;
    }

    if (m_pImgIn == NULL || m_pImgIn->empty())
    {
        AfxMessageBox(_T("No input image for preview!"));
        return;
    }

    CImageDataset imgOut;
    if (CImageProcessingEx::medianFilter(*m_pImgIn, imgOut, m_nWinSize))
    {
        CString strTitle;
        strTitle.Format(_T("Preview: Median Filter (%dx%d)"), m_nWinSize, m_nWinSize);
        
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