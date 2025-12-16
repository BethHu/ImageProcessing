
// myAlgDlg.h : header file
//

#pragma once

#include "RSDIPLib.h"
#include "ImageProcessingEx.h"      // 添加这行
#include "MedianFilterDlg.h" 
#include "BilateralFilter.h"
#include "FourierInputDlg.h"
#include "FourierSpectrumDlg.h"



// CmyAlgDlg dialog
class CmyAlgDlg : public CDialogEx
{
// Construction
public:
	CmyAlgDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MYALG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CImageDataset imgIn;	//读取后的图像
	CImageDataset imgOut;	//待保存的图像


public:
	CString m_strImgInput;
	CString m_strImgOutput;
	afx_msg void OnClickedButtonRead();
	afx_msg void OnClickedButtonWrite();
	afx_msg void OnClickedButtonHist();
	afx_msg void OnClickedButtonHismatch();
	afx_msg void OnClickedButtonMedian();
	afx_msg void OnClickedButtonBilateralfilter();
	afx_msg void OnClickedButtonLaplacian();
	afx_msg void OnClickedButtonFourier();
};
