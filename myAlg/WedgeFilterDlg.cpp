// WedgeFilterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myAlg.h"
#include "WedgeFilterDlg.h"
#include "afxdialogex.h"
#include "FourierTransform.h"

IMPLEMENT_DYNAMIC(CWedgeFilterDlg, CDialogEx)

CWedgeFilterDlg::CWedgeFilterDlg(CFourierSpectrumDlg* pSpectrumDlg, CWnd* pParent)
	: CDialogEx(IDD_WEDGEFILTERDLG, pParent)
	, m_pSpectrumDlg(pSpectrumDlg)
	, m_centerAngleDeg(0.0)
	, m_halfAngleDeg(10.0)
	, m_bSymmetric(TRUE)
	, m_wedgeRadius(0.0)
	, m_maxRadius(0.0)
	, m_bInternalUpdate(FALSE)
{
}

CWedgeFilterDlg::~CWedgeFilterDlg()
{
}

void CWedgeFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWedgeFilterDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CWedgeFilterDlg::OnBnClickedOK)
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW, &CWedgeFilterDlg::OnBnClickedButtonPreview)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON__DETECT, &CWedgeFilterDlg::OnBnClickedButtonDetect)
	ON_EN_CHANGE(IDC_EDIT_WEDGE_RADIUS, &CWedgeFilterDlg::OnEnChangeEditWedgeRadius)
END_MESSAGE_MAP()

BOOL CWedgeFilterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetWindowText(_T("Wedge ?????"));

	// ??????
	CString s;
	double startDeg = m_centerAngleDeg - m_halfAngleDeg;
	double endDeg   = m_centerAngleDeg + m_halfAngleDeg;
	s.Format(_T("%.1f"), startDeg);
	if (GetDlgItem(IDC_EDIT_START_ANGLE)) SetDlgItemText(IDC_EDIT_START_ANGLE, s);
	s.Format(_T("%.1f"), endDeg);
	if (GetDlgItem(IDC_EDIT_END_ANGLE)) SetDlgItemText(IDC_EDIT_END_ANGLE, s);

	// ?? m_maxRadius
	if (m_pSpectrumDlg && m_pSpectrumDlg->IsDFTComputed())
	{
		m_maxRadius = 0.5 * min(m_pSpectrumDlg->GetDFTWidth(), m_pSpectrumDlg->GetDFTHeight());
	}
	else
	{
		m_maxRadius = 100.0;  // ?????
	}
	if (m_maxRadius <= 0.0)
		m_maxRadius = 100.0;

	if (m_wedgeRadius < 0.0)
		m_wedgeRadius = 0.0;

	// ?????????????????
	m_bInternalUpdate = TRUE;

	// Slider 0..100 ??? 0..m_maxRadius
	CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_RADIUS);
		SetWindowText(_T("Wedge stripe removal"));
	{
		pSlider->SetRange(0, 100);
		int pos = (int)floor(m_wedgeRadius * 100.0 / m_maxRadius + 0.5);
		if (pos < 0) pos = 0;
		if (pos > 100) pos = 100;
		pSlider->SetPos(pos);
	}

	// ?????
	s.Format(_T("%.0f"), m_wedgeRadius);
		// compute m_maxRadius

	m_bInternalUpdate = FALSE;

	return TRUE;
}

static inline double rad2deg(double r) { return r * 180.0 / 3.14159265358979323846; }
static inline double deg2rad(double d) { return d * 3.14159265358979323846 / 180.0; }
static inline double angNormDeg(double a)
{
	while (a <= -180) a += 360;
	while (a > 180) a -= 360;
	return a;
}

void CWedgeFilterDlg::ApplyWedgeMask()
{
	if (!m_pSpectrumDlg || !m_pSpectrumDlg->IsDFTComputed())
	{
		AfxMessageBox(_T("DFT data is not available. Please compute spectrum first."));
		return;
	}

	int W = m_pSpectrumDlg->GetDFTWidth();
	int H = m_pSpectrumDlg->GetDFTHeight();
	if (W <= 0 || H <= 0) return;

	// build mask on centered DFT
	const auto& dftC = m_pSpectrumDlg->GetDFTCenteredResult();
	m_filteredDFT.resize(W * H);

	int cx = W / 2, cy = H / 2;
	double center = angNormDeg(m_centerAngleDeg);
	double half   = fabs(m_halfAngleDeg);

	for (int y = 0; y < H; ++y)
	{
		for (int x = 0; x < W; ++x)
		{
			int idx = y * W + x;
			double dx = x - cx;
			double dy = y - cy;
			if (dx == 0.0 && dy == 0.0)
			{
				// keep DC component
				m_filteredDFT[idx] = dftC[idx];
				continue;
			}
			double theta = rad2deg(atan2(dy, dx)); // [-180,180]
			double d1 = fabs(angNormDeg(theta - center));
			double d2 = m_bSymmetric ? fabs(angNormDeg(theta - (center + 180.0))) : 9999.0;
			bool inWedge = (d1 <= half) || (d2 <= half);
			double r = sqrt(dx * dx + dy * dy);
			if (inWedge)
			{
				if (m_wedgeRadius > 0.0 && r > m_wedgeRadius)
					m_filteredDFT[idx] = dftC[idx];
				else
					m_filteredDFT[idx] = std::complex<double>(0.0, 0.0); // zero out
			}
			else
			{
				m_filteredDFT[idx] = dftC[idx];
			}
		}
	}

	// shift center back for IDFT
	CFourierTransform::ShiftDFTCenter(m_filteredDFT, W, H);
}

void CWedgeFilterDlg::OnBnClickedButtonPreview()
{
	// read angles and radius from UI
	CString s;
	double startDeg = 0.0, endDeg = 0.0;
	if (GetDlgItem(IDC_EDIT_START_ANGLE))
	{
		GetDlgItemText(IDC_EDIT_START_ANGLE, s);
		startDeg = _ttof(s);
	}
	if (GetDlgItem(IDC_EDIT_END_ANGLE))
	{
		GetDlgItemText(IDC_EDIT_END_ANGLE, s);
		endDeg = _ttof(s);
	}
	m_centerAngleDeg = (startDeg + endDeg) / 2.0;
	m_halfAngleDeg   = fabs(endDeg - startDeg) / 2.0;
	m_bSymmetric     = TRUE; // symmetric by default

	// radius
	if (GetDlgItem(IDC_EDIT_WEDGE_RADIUS))
	{
		GetDlgItemText(IDC_EDIT_WEDGE_RADIUS, s);
		m_wedgeRadius = _ttof(s);
	}

	if (m_halfAngleDeg < 0.5)  m_halfAngleDeg = 0.5;
	if (m_halfAngleDeg > 90.0) m_halfAngleDeg = 90.0;

	ApplyWedgeMask();

	if (!m_filteredDFT.empty() && m_pSpectrumDlg)
	{
		// build preview spectrum from filtered DFT
		std::vector<std::complex<double>> tmp = m_filteredDFT;
		CFourierTransform::ShiftDFTCenter(tmp,
			m_pSpectrumDlg->GetDFTWidth(),
			m_pSpectrumDlg->GetDFTHeight());
		CImageDataset spec;
		if (CFourierTransform::ComputeMagnitudeSpectrum(tmp, spec,
			m_pSpectrumDlg->GetDFTWidth(),
			m_pSpectrumDlg->GetDFTHeight(),
			true))
		{
			CString title;
			title.Format(_T("Wedge preview: theta = %.1f, half = %.1f %s"),
				m_centerAngleDeg,
				m_halfAngleDeg,
				m_bSymmetric ? _T("(symmetric)") : _T(""));
			CImageDisplay::show(spec, this, title, 1, 1, 1, 0);
		}
	}
}

void CWedgeFilterDlg::OnBnClickedOK()
{
	// ensure m_filteredDFT is generated with current parameters
	OnBnClickedButtonPreview();
	if (m_filteredDFT.empty())
	{
		AfxMessageBox(_T("Filtering failed. Please check parameters."));
		return;
	}
	CDialogEx::OnOK();
}

void CWedgeFilterDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// Slider 0..100 -> radius 0..m_maxRadius
	CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_RADIUS);
	if (pSlider && (CScrollBar*)pSlider == pScrollBar)
	{
		if (m_bInternalUpdate)
		{
			CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
			return;
		}

		if (m_maxRadius <= 0.0)
		{
			if (m_pSpectrumDlg && m_pSpectrumDlg->IsDFTComputed())
				m_maxRadius = 0.5 * min(m_pSpectrumDlg->GetDFTWidth(), m_pSpectrumDlg->GetDFTHeight());
			else
				m_maxRadius = 100.0;
		}

		int pos = pSlider->GetPos();
		m_wedgeRadius = (double)pos * m_maxRadius / 100.0;
		CString s;

		// lock while updating edit box to avoid recursive EN_CHANGE
		m_bInternalUpdate = TRUE;
		s.Format(_T("%.0f"), m_wedgeRadius);
		SetDlgItemText(IDC_EDIT_WEDGE_RADIUS, s);
		m_bInternalUpdate = FALSE;
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CWedgeFilterDlg::OnEnChangeEditWedgeRadius()
{
	// text -> Slider
	if (m_bInternalUpdate)
		return;

	CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_RADIUS);
	if (!pSlider) return;

	if (m_maxRadius <= 0.0)
	{
		if (m_pSpectrumDlg && m_pSpectrumDlg->IsDFTComputed())
			m_maxRadius = 0.5 * min(m_pSpectrumDlg->GetDFTWidth(), m_pSpectrumDlg->GetDFTHeight());
		else
			m_maxRadius = 100.0;
	}

	CString s;
	GetDlgItemText(IDC_EDIT_WEDGE_RADIUS, s);
	m_wedgeRadius = _ttof(s);

	if (m_wedgeRadius < 0.0)        m_wedgeRadius = 0.0;
	if (m_wedgeRadius > m_maxRadius) m_wedgeRadius = m_maxRadius;

	// lock while updating edit box and slider to avoid recursion
	m_bInternalUpdate = TRUE;

	s.Format(_T("%.0f"), m_wedgeRadius);
	SetDlgItemText(IDC_EDIT_WEDGE_RADIUS, s);

	int pos = (int)floor(m_wedgeRadius * 100.0 / m_maxRadius + 0.5);
	if (pos < 0) pos = 0;
	if (pos > 100) pos = 100;
	pSlider->SetRange(0, 100);
	pSlider->SetPos(pos);

	m_bInternalUpdate = FALSE;
}

static double s_pi() { return 3.14159265358979323846; }

double CWedgeFilterDlg::DetectDominantAngleDeg(int bins)
{
	if (!m_pSpectrumDlg || !m_pSpectrumDlg->IsDFTComputed()) return 0.0;
	const auto& dftC = m_pSpectrumDlg->GetDFTCenteredResult();
	int W = m_pSpectrumDlg->GetDFTWidth();
	int H = m_pSpectrumDlg->GetDFTHeight();
	int cx = W / 2, cy = H / 2;
	std::vector<double> hist(bins, 0.0);
	double rMax = m_wedgeRadius > 0.0 ? m_wedgeRadius : 0.5 * min(W, H);
	double rMin = 2.0; // skip very small radius near the center

	for (int y = 0; y < H; ++y)
	{
		for (int x = 0; x < W; ++x)
		{
			double dx = x - cx;
			double dy = y - cy;
			double r  = sqrt(dx * dx + dy * dy);
			if (r < rMin || r > rMax) continue;
			double theta = atan2(dy, dx); // [-pi,pi]
			int b = (int)floor(((theta + s_pi()) / (2 * s_pi())) * bins);
			if (b < 0)      b = 0;
			if (b >= bins) b = bins - 1;
			double mag = std::abs(dftC[y * W + x]);
			hist[b] += mag;
		}
	}
	int   idxMax = 0;
	double vMax  = hist[0];
	for (int i = 1; i < bins; ++i)
	{
		if (hist[i] > vMax)
		{
			vMax  = hist[i];
			idxMax = i;
		}
	}
	double angle = ((idxMax + 0.5) / bins) * 360.0 - 180.0; // bin ???
	return angle;
}

void CWedgeFilterDlg::OnBnClickedButtonDetect()
{
	double ang      = DetectDominantAngleDeg(360);
	double startDeg = ang - 10.0;
	double endDeg   = ang + 10.0;
	CString s;
	s.Format(_T("%.1f"), startDeg);
	if (GetDlgItem(IDC_EDIT_START_ANGLE)) SetDlgItemText(IDC_EDIT_START_ANGLE, s);
	s.Format(_T("%.1f"), endDeg);
	if (GetDlgItem(IDC_EDIT_END_ANGLE))   SetDlgItemText(IDC_EDIT_END_ANGLE, s);
	CString msg;
	msg.Format(_T("Detected main noise angle: %.1f deg\n(Already filled into start/end angle, default +/-10 deg)"), ang);
	AfxMessageBox(msg, MB_ICONINFORMATION);
}

