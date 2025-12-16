// FourierSpectrumDlg.h
#pragma once
#include "afxdialogex.h"
#include "RSDIPLib.h"
#include "FourierTransform.h"
#include <vector>
#include <complex>

class CFourierSpectrumDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CFourierSpectrumDlg)

public:
    CFourierSpectrumDlg(CImageDataset* pInputImage = nullptr, CWnd* pParent = nullptr);
    virtual ~CFourierSpectrumDlg();
	CImageDataset* GetOriginalImage() { return m_pInputImage; }
    CImageDataset& GetReconstructedImage() { return m_reconstructedImage; }
    CImageDataset& GetErrorImage() { return m_errorImage; }
    double GetMSE() const { return m_mse; }
    double GetPSNR() const { return m_psnr; }
    double GetMaxError() const { return m_maxError; }
	int ComputeInverseDFT();
    void ShowErrorAnalysis();
    
    // 设置是否自动显示反变换对话框
    void SetAutoShowInverseDFT(BOOL bAutoShow) { m_bAutoShowInverseDFT = bAutoShow; }

    enum { IDD = IDD_FOURIERSPECTRUMDLG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedCheckAmpltitude();
    afx_msg void OnBnClickedCheckPhase();
    afx_msg void OnBnClickedCheckAngular();
    afx_msg void OnBnClickedCheckRadial();
    afx_msg void OnBnClickedCheckNormalizeAmp();
    afx_msg void OnBnClickedCheckLogTransform();
    afx_msg void OnBnClickedButtonApply();
    afx_msg void OnBnClickedRadioPhaseRange1();
    afx_msg void OnBnClickedRadioPhaseRange2();
    afx_msg void OnBnClickedRadioPhaseRange3();
    afx_msg void OnPaint();
	afx_msg void OnDestroy();
	void ifftshift(std::vector<std::complex<double>>& data, int w, int h);
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()


private:
    void ComputeDFT();
    void UpdateSpectrumDisplay();
    void NormalizeImageDisplay();
    void ApplyLogTransform();
    void UpdateParameterControls();  // 添加这个函数声明
	void DisplayImageDirectly(CDC& dc, CRect& imageRect);
	void fftshift(std::vector<std::complex<double>>& data, int width, int height); 

public:
    // DFT 相关数据 - 改为 public 以供其他对话框使用
    std::vector<std::complex<double>> m_dftResult;
    int m_dftWidth;
    int m_dftHeight;

private:
    CImageDataset* m_pInputImage;
	std::vector<std::complex<double>> m_dftResultCentered;
    CImageDataset m_displayImage;
    
    // 添加这4个字符串成员变量
    CString m_strDisplayMin;
    CString m_strDisplayMax;
    CString m_strLogBase;
    CString m_strLogOffset;
    
    // Spectrum type flags
    BOOL m_bShowAmplitude;
    BOOL m_bShowPhase;
    BOOL m_bShowAngular;
    BOOL m_bShowRadial;
    
    // Amplitude spectrum parameters
    double m_dDisplayMin;
    double m_dDisplayMax;
    double m_dLogBase;
    double m_dLogOffset;
    BOOL m_bNormalizeAmp;
    BOOL m_bLogTransform;
    
    // Phase spectrum parameters
    int m_nPhaseRange;
    
    // Angular spectrum parameters
    int m_nAngularBins;
    double m_dAngularRange;
    
    // Radial spectrum parameters
    int m_nRadialBins;
    double m_dMaxRadius;

    CImageDataset m_reconstructedImage;  // 重建图像
    CImageDataset m_errorImage;          // 误差图像
    double m_mse;                        // 均方误差
    double m_psnr;                       // PSNR
    double m_maxError;                   // 最大误差
    BOOL m_bAutoShowInverseDFT;          // 是否自动显示反变换对话框（默认TRUE）
public:
	BOOL m_bCenter;
	std::vector<std::complex<double>>& GetDFTResult() { return m_dftResult; }
	int GetDFTWidth() const { return m_dftWidth; }
	int GetDFTHeight() const { return m_dftHeight; }
	BOOL IsDFTComputed() const { return m_bDFTComputed; }
	BOOL m_bDFTComputed; 
};