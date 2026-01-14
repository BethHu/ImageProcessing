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

	// 用于接收过滤后的 DFT 数据
	void SetFilteredDFT(std::vector<std::complex<double>>* pFilteredDFT)
	{
		m_pFilteredDFT = pFilteredDFT;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	CFourierSpectrumDlg* m_pParentDlg;
	std::vector<std::complex<double>>* m_pFilteredDFT;  // 指向过滤后的 DFT 数据
	
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