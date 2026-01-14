//IdealFilterDlg.h
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

    enum FilterType
    {
        FILTER_IDEAL_HP = 0,
        FILTER_BUTTERWORTH_HP = 1,
        FILTER_GAUSSIAN_HP = 2
    };

    std::vector<std::complex<double>> m_filteredDFT;
    double m_dCutoffFrequency;
    int m_nOrder;               
    FilterType m_filterType;    
    double m_boostBase;         

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedButtonPreview();
    afx_msg void OnBnClickedOK();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnBnClickedRadioIdeal();
    afx_msg void OnBnClickedRadioButterworth();
    afx_msg void OnBnClickedRadioGaussian();

private:
    void ApplyIdealHighPassFilter();
    void ApplyButterworthHighPassFilter();
    void ApplyGaussianHighPassFilter();
    void UpdateControlEnableState();

private:
    CFourierSpectrumDlg* m_pSpectrumDlg;
};