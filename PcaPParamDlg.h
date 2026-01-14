#pragma once

#include "afxdialogex.h"
#include "resource.h"
#include <vector>

// 兜底：确保资源ID在未包含resource.h时也有定义（值与resource.h一致）
#ifndef IDD_DIALOG_PCA_P
#define IDD_DIALOG_PCA_P 133
#endif
// 控件 ID 兜底
#ifndef IDC_RADIO_PCA_ONLY
#define IDC_RADIO_PCA_ONLY 1035
#endif
#ifndef IDC_RADIO_SINGLE_P
#define IDC_RADIO_SINGLE_P 1018
#endif
#ifndef IDC_RADIO_MULTI_P
#define IDC_RADIO_MULTI_P 1019
#endif
#ifndef IDC_EDIT_P_VALUE
#define IDC_EDIT_P_VALUE 1020
#endif

class CPcaPParamDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPcaPParamDlg)

public:
	CPcaPParamDlg(CWnd* pParent = nullptr);
	virtual ~CPcaPParamDlg();

	enum { IDD = IDD_DIALOG_PCA_P };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

public:
	int m_nSingleP;
	std::vector<int> m_vecMultiP;
	int m_bIsSingleMode;
	CString m_strPInput;

	void SetBandCount(int nBandCount) { m_nBandCount = nBandCount; }
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioMode();
	virtual void OnOK();

private:
	int m_nBandCount;
};
