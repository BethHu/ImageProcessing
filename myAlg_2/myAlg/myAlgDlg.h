// myAlgDlg.h : header file
//

#pragma once

#include "afxdialogex.h"  // 
#include "RSDIPLib.h"     // 图像库头文件


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
	CImageDataset imgIn;   // 读取后的图像
	CImageDataset imgOut;  // 待保存的图像
	CImageDataset imgRef;   // 参考图像（直方图匹配需要）
public:
	CString m_strlmgInput;                  // 输入图像
	CString m_strlmgOutput;                 // 输出图像
	CString m_strImgRef;                    //参考图像
	afx_msg void OnClickedButtonRead();     // 读取按钮
	afx_msg void OnClickedButtonWrite();    // 保存按钮
	afx_msg void OnClickedButtonHisteq();   //直方图均衡化按钮
	afx_msg void OnClickedButtonHistmatch();//直方图匹配按钮
	afx_msg void OnClickedButtonReadRef();  //读取参考图像按钮
	afx_msg void OnClickedButtonMedian();   //进行中值滤波按钮
	afx_msg void OnClickedButtonBilateral();//进行双边滤波按钮
	afx_msg void OnClickedButtonLaplacian();//进行拉普拉斯算子
	afx_msg void OnClickedButtonPca();       //主成分变换
	afx_msg void OnClickedButtonRgbIhs();    //RGB-IHS变换
	afx_msg void OnClickedButtonCanny();     //Canny边缘检测
};