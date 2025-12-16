#pragma once
#include "afxdialogex.h"
#include "FourierSpectrumDlg.h"
#include <complex>
#include <vector>

class CIdealFilterDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CIdealFilterDlg)

public:
    CIdealFilterDlg(CFourierSpectrumDlg* pSpectrumDlg = nullptr, 
                   CWnd* pParent = nullptr);
    virtual ~CIdealFilterDlg();

    enum { IDD = IDD_IDEALFILTERDLG };

    std::vector<std::complex<double>> m_filteredDFT;
    double m_dCutoffFrequency;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedButtonPreview();
    afx_msg void OnBnClickedOK();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

private:
    void ApplyIdealHighPassFilter();

private:
    CFourierSpectrumDlg* m_pSpectrumDlg;
};