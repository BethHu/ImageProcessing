// PcaChartDlg.cpp : PCA RMSE 曲线对话框实现

#include "stdafx.h"
#include "myAlg.h"
#include "PcaChartDlg.h"
#include "afxdialogex.h"
#include <algorithm>
#include <atlimage.h>

#define ID_BTN_SAVE 3001

IMPLEMENT_DYNAMIC(CPcaChartDlg, CDialogEx)

CPcaChartDlg::CPcaChartDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CPcaChartDlg::IDD, pParent)
{
}

CPcaChartDlg::~CPcaChartDlg()
{
}

void CPcaChartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void CPcaChartDlg::SetData(const std::vector<int> &vecP, const std::vector<double> &vecRmse)
{
	m_vecP = vecP;
	m_vecRmse = vecRmse;
}

BEGIN_MESSAGE_MAP(CPcaChartDlg, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(ID_BTN_SAVE, &CPcaChartDlg::OnBnClickedButtonSave)
END_MESSAGE_MAP()

BOOL CPcaChartDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_font.CreatePointFont(90, _T("Arial"));

	CRect rectDlg;
	GetClientRect(&rectDlg);
	CRect rectBtn;
	rectBtn.SetRect(rectDlg.right - 180, rectDlg.bottom - 55, rectDlg.right - 20, rectDlg.bottom - 15);
	m_btnSave.Create(_T("Save Image"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rectBtn, this, ID_BTN_SAVE);
	m_btnSave.SetFont(&m_font);

	return TRUE;
}

void CPcaChartDlg::OnPaint()
{
	CPaintDC dc(this);
	CWnd *pWnd = GetDlgItem(IDC_STATIC_CHART);
	if (!pWnd) return;
	CRect rect;
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.bottom -= 50;
	DrawChart(&dc, rect);
}

void CPcaChartDlg::DrawChart(CDC *pDC, CRect rect)
{
	pDC->FillSolidRect(rect, RGB(255, 255, 255));
	pDC->FrameRect(rect, &CBrush(RGB(0, 0, 0)));

	if (m_vecP.empty() || m_vecRmse.empty()) return;

	double maxRmse = *std::max_element(m_vecRmse.begin(), m_vecRmse.end());
	double minRmse = *std::min_element(m_vecRmse.begin(), m_vecRmse.end());
	double range = maxRmse - minRmse;
	if (range < 1e-6) range = 1.0;
	double yMax = maxRmse + range * 0.1;
	double yMin = minRmse - range * 0.1;
	if (yMin < 0) yMin = 0;

	int maxP = *std::max_element(m_vecP.begin(), m_vecP.end());
	int minP = *std::min_element(m_vecP.begin(), m_vecP.end());

	int marginL = 70;
	int marginR = 40;
	int marginT = 40;
	int marginB = 60;

	int plotLeft = rect.left + marginL;
	int plotRight = rect.right - marginR;
	int plotTop = rect.top + marginT;
	int plotBottom = rect.bottom - marginB;
	int plotW = plotRight - plotLeft;
	int plotH = plotBottom - plotTop;

	CPen penGrid(PS_DASH, 1, RGB(200, 200, 200));
	CPen penAxis(PS_SOLID, 2, RGB(0, 0, 0));
	CFont *pOldFont = pDC->SelectObject(&m_font);

	int nTicks = 5;
	for (int i = 0; i <= nTicks; i++)
	{
		int y = plotBottom - (int)((double)i / nTicks * plotH);
		double val = yMin + (double)i / nTicks * (yMax - yMin);

		CPen *pOldPen = pDC->SelectObject(&penGrid);
		pDC->MoveTo(plotLeft, y);
		pDC->LineTo(plotRight, y);
		pDC->SelectObject(pOldPen);

		CString strVal;
		strVal.Format(_T("%.2f"), val);
		CRect rectText(rect.left, y - 10, plotLeft - 5, y + 10);
		pDC->DrawText(strVal, rectText, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	}

	for (size_t i = 0; i < m_vecP.size(); i++)
	{
		int pVal = m_vecP[i];
		int x = plotLeft + (int)((double)(pVal - minP) / (maxP - minP) * plotW);
		if (maxP == minP) x = plotLeft + plotW / 2;

		CPen *pOldPen = pDC->SelectObject(&penGrid);
		pDC->MoveTo(x, plotTop);
		pDC->LineTo(x, plotBottom);
		pDC->SelectObject(pOldPen);

		CString strVal;
		strVal.Format(_T("%d"), pVal);
		CRect rectText(x - 20, plotBottom + 5, x + 20, plotBottom + 25);
		pDC->DrawText(strVal, rectText, DT_CENTER | DT_TOP | DT_SINGLELINE);
	}

	CPen *pOldPen = pDC->SelectObject(&penAxis);
	pDC->MoveTo(plotLeft, plotTop);
	pDC->LineTo(plotLeft, plotBottom);
	pDC->MoveTo(plotLeft, plotBottom);
	pDC->LineTo(plotRight, plotBottom);
	pDC->SelectObject(pOldPen);

	pDC->TextOut(plotLeft + plotW / 2 - 40, plotBottom + 30, _T("Principal Components (p)"));
	pDC->TextOut(rect.left + 10, rect.top + 5, _T("RMSE"));

	CPen penLine(PS_SOLID, 2, RGB(0, 0, 255));
	CBrush brushPoint(RGB(255, 0, 0));
	pOldPen = pDC->SelectObject(&penLine);
	CBrush *pOldBrush = pDC->SelectObject(&brushPoint);

	CPoint prevPoint;
	for (size_t i = 0; i < m_vecP.size(); i++)
	{
		int pVal = m_vecP[i];
		double rmse = m_vecRmse[i];

		int x = plotLeft + (int)((double)(pVal - minP) / (maxP - minP) * plotW);
		if (maxP == minP) x = plotLeft + plotW / 2;
		int y = plotBottom - (int)((rmse - yMin) / (yMax - yMin) * plotH);

		if (i > 0)
		{
			pDC->MoveTo(prevPoint);
			pDC->LineTo(x, y);
		}
		prevPoint = CPoint(x, y);

		int r = 3;
		pDC->Ellipse(x - r, y - r, x + r, y + r);

		CString strVal;
		strVal.Format(_T("%.2f"), rmse);
		pDC->TextOut(x + 5, y - 15, strVal);
	}

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldFont);
}

void CPcaChartDlg::OnBnClickedButtonSave()
{
	CFileDialog dlg(FALSE, _T("png"), _T("PCA_Error_Chart.png"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("PNG Image (*.png)|*.png|Bitmap (*.bmp)|*.bmp||"), this);
	if (dlg.DoModal() == IDOK)
	{
		CString filePath = dlg.GetPathName();
		CWnd *pWnd = GetDlgItem(IDC_STATIC_CHART);
		if (!pWnd) return;
		CRect rect;
		pWnd->GetWindowRect(&rect);
		int w = rect.Width();
		int h = rect.Height();

		CImage image;
		image.Create(w, h, 24);
		CDC *pDC = CDC::FromHandle(image.GetDC());

		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		CBitmap bmp;
		bmp.CreateCompatibleBitmap(pDC, w, h);
		CBitmap *pOldBmp = memDC.SelectObject(&bmp);
		memDC.FillSolidRect(0, 0, w, h, RGB(255, 255, 255));
		CRect drawRect(0, 0, w, h);
		DrawChart(&memDC, drawRect);
		pDC->BitBlt(0, 0, w, h, &memDC, 0, 0, SRCCOPY);
		memDC.SelectObject(pOldBmp);
		image.ReleaseDC();

		HRESULT hr = image.Save(filePath);
		if (SUCCEEDED(hr))
			AfxMessageBox(_T("Image saved successfully!"));
		else
			AfxMessageBox(_T("Failed to save image."));
	}
}
