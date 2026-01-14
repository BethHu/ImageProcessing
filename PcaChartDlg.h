#pragma once

#include "afxdialogex.h"
#include "resource.h"
#include <vector>

#ifndef IDD_DIALOG_PCA_CHART
#define IDD_DIALOG_PCA_CHART 136
#endif

// 控件 ID 兜底
#ifndef IDC_STATIC_CHART
#define IDC_STATIC_CHART 1036
#endif

class CPcaChartDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPcaChartDlg)

public:
	CPcaChartDlg(CWnd* pParent = nullptr);
	virtual ~CPcaChartDlg();

	enum { IDD = IDD_DIALOG_PCA_CHART };

	void SetData(const std::vector<int> &vecP, const std::vector<double> &vecRmse);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonSave();
	DECLARE_MESSAGE_MAP()

private:
	void DrawChart(CDC *pDC, CRect rect);

	std::vector<int> m_vecP;
	std::vector<double> m_vecRmse;
	CButton m_btnSave;
	CFont m_font;
};
