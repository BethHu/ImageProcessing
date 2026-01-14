#pragma once

#include <vector>
#include "RSDIPLib.h"

// COtsuDlg 对话框

class COtsuDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COtsuDlg)

public:
	COtsuDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COtsuDlg();

	// 由主对话框传入当前输入图像
	void SetInputImage(CImageDataset* pImage) { m_pInputImage = pImage; }
	void SetOutputImage(CImageDataset* pImage) { m_pOutputImage = pImage; }

// 对话框数据
	enum { IDD = IDD_OTSUDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

protected:
	// 输入图像指针（不管理内存）
	CImageDataset* m_pInputImage;
	// 输出图像指针（指向主对话框的 imgOut，不管理内存）
	CImageDataset* m_pOutputImage;

	// 内部辅助函数
	void UpdateChannelControls();
	void UpdateSmoothControls();
	void UpdateThresholdControls();

	BOOL BuildGrayImage(CImageDataset& grayImg, bool bAutoGray, int nRgbChannel);
	BOOL ApplyPreprocess(CImageDataset& src, CImageDataset& dst,
		int nSmoothMode, int nKernelSize, double dSigma);
	BOOL GaussianSmooth(CImageDataset& src, CImageDataset& dst,
		int kernelSize, double sigma);
	BOOL OtsuSingle(CImageDataset& img, int& threshold);
	BOOL OtsuMulti(CImageDataset& img, int levelCount, std::vector<int>& thresholds);
	BOOL BuildHistogramImage(CImageDataset& img,
		const std::vector<int>& thresholds, CImageDataset& histImg);
	BOOL ComputeOtsuSegmentation(CImageDataset& preImg,
		CImageDataset& segImg,
		std::vector<int>& thresholds,
		BOOL& bMulti);

	// 消息响应
	afx_msg void OnBnClickedRadioGrayscale();
	afx_msg void OnBnClickedRadioRgb();
	afx_msg void OnCbnSelchangeComboSmooth();
	afx_msg void OnBnClickedRadioSingleThreshold();
	afx_msg void OnBnClickedRadioMultiThreshold();
	afx_msg void OnBnClickedButtonPreview();
};
