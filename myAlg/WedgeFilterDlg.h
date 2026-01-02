#pragma once
#include "afxdialogex.h"
#include "FourierSpectrumDlg.h"
#include <vector>
#include <complex>

// WedgeFilterDlg: zero out a wedge in centered spectrum to remove stripes
class CWedgeFilterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWedgeFilterDlg)
public:
	CWedgeFilterDlg(CFourierSpectrumDlg* pSpectrumDlg = nullptr, CWnd* pParent = nullptr);
	virtual ~CWedgeFilterDlg();

	enum { IDD = IDD_WEDGEFILTERDLG };

	// Output: filtered (non-centered) DFT for IDFT
	std::vector<std::complex<double>> m_filteredDFT;

	// Parameters
	double m_centerAngleDeg;   // center angle in degrees
	double m_halfAngleDeg;     // half angle in degrees
	BOOL   m_bSymmetric;       // symmetric around +/- centerAngle
	double m_wedgeRadius;      // wedge radius in pixels (<=0 means unlimited)
	double m_maxRadius;        // max selectable radius in pixels (min(W,H)/2)

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonPreview();
	afx_msg void OnBnClickedOK();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonDetect();
	afx_msg void OnEnChangeEditWedgeRadius();

private:
	void ApplyWedgeMask();
	void UpdateRadiusControls(bool fromSlider);
	double DetectDominantAngleDeg(int bins = 360);
	CFourierSpectrumDlg* m_pSpectrumDlg;
	BOOL m_bInternalUpdate;   // guard for internal UI updates to avoid recursion
};

