// InverseDFTDlg.h
#pragma once

#include "FourierSpectrumDlg.h"  // 包含父对话框的头文件
#include "Image.h"

// 不需要前向声明了，因为已经包含了头文件

class CInverseDFTDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInverseDFTDlg)

public:
	CInverseDFTDlg(CFourierSpectrumDlg* pParentDlg, CWnd* pParent = nullptr);
	virtual ~CInverseDFTDlg();

	enum { IDD = IDD_INVERSEDFTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	CFourierSpectrumDlg* m_pParentDlg;
	CImageDataset m_reconstructedImage;
	CImageDataset m_errorImage;
	double m_mse;
	double m_psnr;
	double m_maxError;

protected:
	afx_msg void OnBnClickedButtonInverse();
	afx_msg void OnBnClickedButtonError();
	afx_msg void OnBnClickedButtonCompare();

	DECLARE_MESSAGE_MAP()
};